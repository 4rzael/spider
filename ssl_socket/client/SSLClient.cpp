#include <errno.h>
#include <algorithm>

#ifndef _WIN32
# include <sys/select.h>
#endif

#include <iostream>
#include <fcntl.h>

#include "SSLClient.hpp"

namespace Socket
{
	static inline int getError()
	{
#ifdef _WIN32
		return WSAGetLastError();
#else
		return errno;
#endif
	}

	static bool setNonBloccking(SOCKET sock)
	{
#ifdef _WIN32
		u_long mode = 1;
		if ((ioctlsocket(sock, FIONBIO, &mode)) == -1)
			return false;
#else
		if ((fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK)) == -1)
			return false;
#endif
		return true;
	}

	SSLClient::SSLClient()
	{
		_stateThread = std::thread();
		_OnDisconnect = NULL;
		_OnReadPossible = NULL;
		_OnWritePossible = NULL;
		_OnStart = NULL;
		_isRunning = false;
		_timeout.tv_sec = 10;
		_timeout.tv_usec = 0;
		_granularity = 1;
		_internal_read_buffer.reserve(4096);
	}

	SSLClient::~SSLClient()
	{
	}

	void  SSLClient::start(std::string address, int port)
	{
		SOCKADDR_IN	addr;
		PROTOENT	*proto;

		// port checking
		if (port <= 0 || port > 0xFFFF)
			throw SocketCreateError(std::string("SSLclient : bad port"));
		_address = address;
		_port = port;

		// create socket
		proto = getprotobyname("tcp");

		if (!initSSL())
			throw std::exception();

		if ((_fd = socket(AF_INET, SOCK_STREAM, proto ? proto->p_proto : 0)) == -1)
			throw SocketCreateError(std::string("SSLclient : ") +
				std::string(strerror(getError())));


		// bind
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		if (inet_pton(AF_INET, address.c_str(), &addr.sin_addr) < 0)
			throw SocketConnectError("SSLclient : " + std::string(strerror(getError())));

		if (connect(_fd, (SOCKADDR *)&addr, sizeof(addr)) < 0)
		{
			if (errno != EINPROGRESS)
				throw SocketConnectError("SSLclient : " + std::string(strerror(getError())));
		}

		SSL_set_fd(_ssl, _fd);

		if ( SSL_connect(_ssl) != 1 )
		  BIO_printf(_outbio, "Error: Could not build a SSL session to: %s:%d.\n", _address.c_str(), _port);
		else
		  BIO_printf(_outbio, "Successfully enabled SSL/TLS session to: %s:%d.\n", _address.c_str(), _port);

		if (!setNonBloccking(_fd))
			throw SocketCreateError(std::string("SSLclient : ") +
				std::string("cannot set non blocking mode for the SSLclient fd"));

		// create select() sets
		FD_ZERO(&_fd_set);
		FD_SET(_fd, &_fd_set);

		// run the event loop
		_isRunning = true;
		if (!_stateThread.joinable())
			_stateThread = std::thread(&SSLClient::stateChecker, this);

		// start event
		if (_OnStart)
			_OnStart(*this, _address, _port);
	}

	void  SSLClient::stop()
	{
		_isRunning = false;
		if (_stateThread.joinable())
			_stateThread.join();
	}

	void  SSLClient::setTimeout(float t)
	{
		_timeout.tv_sec = t / 1000;
		_timeout.tv_usec = (int)(t * 1000) % (1000 * 1000);
	}

	float SSLClient::getTimeout() const
	{
		return (_timeout.tv_sec * 1000) + ((float)(_timeout.tv_usec) / 1000);
	}

	void  SSLClient::OnDisconnect(std::function < void (Socket::SSLClient &) > const &callback)
	{
		_OnDisconnect = callback;
	}

	void  SSLClient::OnReadPossible(std::function < void (Socket::SSLClient &, size_t) > const &callback)
	{
		_OnReadPossible = callback;
	}

	void  SSLClient::OnWritePossible(std::function < void (Socket::SSLClient &) > const &callback)
	{
		_OnWritePossible = callback;
	}

	void  SSLClient::OnStart(std::function < void (Socket::SSLClient &, std::string const &, int) > const &callback)
	{
		_OnStart = callback;
	}

	int  SSLClient::read(void *buffer, size_t size)
	{
		// read from the SSL socket
		prefetch_data();

		if (_internal_read_buffer.empty())
			return 0;
		// copy the internal buffer to the given one
		size_t cpy_size = size < _internal_read_buffer.size() ? size : _internal_read_buffer.size();
		memcpy(buffer, _internal_read_buffer.data(), cpy_size);
		// delete copied data from the internal buffer
		auto cpy_buffer = _internal_read_buffer;
		auto cpy_buffer_iter = cpy_buffer.begin();
		for (int i = 0; i < cpy_size && cpy_buffer_iter != cpy_buffer.end();) {
			++i;
			++cpy_buffer_iter;
		}
		_internal_read_buffer = std::vector<char>(cpy_buffer_iter, cpy_buffer.end());
		if (_internal_read_buffer.size() < 4096)
			_internal_read_buffer.reserve(4096);

		return cpy_size;
	}

	int  SSLClient::write(void const *buffer, size_t size)
	{
		int tmp = 0;
		size_t s = 0;

		// if ((tmp = ::send(_fd, (char *)buffer, size, 0)) == -1)
		// {
		// 	if (errno == ECONNRESET)
		// 		disconnect();
		// 	else
		// 		throw SocketIOError(std::string(strerror(getError())));
		// }

		while (s < size && (tmp = SSL_write(_ssl, (char *)buffer + s, size - s)) > 0) {
			s += tmp;
		}

		if (tmp < 0)
		{
			if (SSL_get_error(_ssl, tmp) == SSL_ERROR_WANT_READ ||
				SSL_get_error(_ssl, tmp) == SSL_ERROR_WANT_WRITE)
				return s;
			else
				disconnect();
		}

		return s;
	}

	void SSLClient::prefetch_data()
	{
		int tmp = 0;
		char buffer[4096];

		while ((tmp = SSL_read(_ssl, buffer, 4096)) > 0)
		{
			// page_size optimization : 4096 bytes page
			if ((_internal_read_buffer.size() + tmp) % 4096 < tmp)
				_internal_read_buffer.reserve((_internal_read_buffer.size() / 4096 + 1) * 9096 + 4096);
			// add data to the internal buffer
			_internal_read_buffer.reserve(_internal_read_buffer.size() + tmp);
			for (int i = 0; i < tmp; ++i)
				_internal_read_buffer.push_back(buffer[i]);
		}

		if (tmp < 0)
		{
			// nothing to read anymore
			if (SSL_get_error(_ssl, tmp) == SSL_ERROR_WANT_READ ||
				SSL_get_error(_ssl, tmp) == SSL_ERROR_WANT_WRITE)
				return ;
			// error
			else
				disconnect();
		}
	}

	size_t SSLClient::bytesAvailables()
	{
		prefetch_data();

		return _internal_read_buffer.size();
	}

	void SSLClient::disconnect()
	{
		closesocket(_fd);
		if (_OnDisconnect)
			_OnDisconnect(*this);
	}

	void SSLClient::stateChecker()
	{
		int fd;
		int nb_fd;
		fd_set read_set, write_set;
		timeval timeout;

		// event loop
		while (_isRunning)
		{
			SLEEPMS(_granularity);
			memcpy(&read_set, &_fd_set, sizeof(_fd_set));
			memcpy(&write_set, &_fd_set, sizeof(_fd_set));

			timeout = _timeout;
			// wait for an event
			nb_fd = select(_fd + 1, &read_set, &write_set, NULL, &timeout);
			if (nb_fd < 0 && errno != EINTR)
				throw SocketIOError(std::string(strerror(getError())));

			if (FD_ISSET(_fd, &read_set))
			{
			// read event
				if (bytesAvailables() == 0)
				{
					break;
				}
				else if (_OnReadPossible)
					_OnReadPossible(*this, bytesAvailables());
			}

			// write event
			if (FD_ISSET(_fd, &write_set))
			{
				if (_OnWritePossible)
					_OnWritePossible(*this);
			}
		}
		disconnect();
	}

	bool SSLClient::initSSL()
	{
		OpenSSL_add_all_algorithms();
		ERR_load_BIO_strings();
		ERR_load_crypto_strings();
		SSL_load_error_strings();

		_certbio = BIO_new(BIO_s_file());
		_outbio  = BIO_new_fp(stdout, BIO_NOCLOSE);

		if (SSL_library_init() < 0)
		{
			BIO_printf(_outbio, "Could not initialize the OpenSSL library !\n");
			return false;
		}
		_method = SSLv23_client_method();
		if ( (_ctx = SSL_CTX_new(_method)) == NULL)
		{
		  BIO_printf(_outbio, "Unable to create a new SSL context structure.\n");
		  return false;
		}
		SSL_CTX_set_options(_ctx, SSL_OP_NO_SSLv2);

		_ssl = SSL_new(_ctx);

		return true;
	}

};
