#include <errno.h>
#include <algorithm>

#ifndef _WIN32
# include <sys/select.h>
#endif

#include <iostream>
#include <fcntl.h>

#include "SSLServer.hpp"

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

	SSLServer::SSLServer(std::string certificate, std::string privateKey)
	{
		_stateThread = std::thread();
		_OnConnect = NULL;
		_OnDisconnect = NULL;
		_OnReadPossible = NULL;
		_OnWritePossible = NULL;
		_OnStart = NULL;
		_isRunning = false;
		_timeout.tv_sec = 10;
		_timeout.tv_usec = 0;
		_granularity = 1;
		_certPath = certificate;
		_pKeyPath = privateKey;
	}

	SSLServer::~SSLServer()
	{
	}

	void  SSLServer::start(int port, size_t maxClients)
	{
		SOCKADDR_IN	addr;
		PROTOENT	*proto;

		// port checking
		if (port <= 0 || port > 0xFFFF)
			throw SocketCreateError(std::string("SSLserver : bad port"));
		_port = port;
		_maxClients = maxClients;

		initSSL();

		// create socket
		proto = getprotobyname("tcp");

		if ((_fd = socket(AF_INET, SOCK_STREAM, proto ? proto->p_proto : 0)) == -1)
			throw SocketCreateError(std::string("SSLserver : ") +
				std::string(strerror(getError())));

		if (!setNonBloccking(_fd))
			throw SocketCreateError(std::string("SSLserver : ") +
				std::string("cannot set non blocking mode for the SSLserver fd"));

		// bind
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port = htons(port);

		if (bind(_fd, (sockaddr *)(&addr), sizeof(addr)) == -1)
			throw SocketConnectError("SSLserver : " + std::string(strerror(getError())));
		if (listen(_fd, _maxClients) == -1)
			throw SocketConnectError("SSLserver : " + std::string(strerror(getError())));

		// create select() sets
		FD_ZERO(&_fd_set);
		_max_fd = _fd;
		FD_SET(_fd, &_fd_set);

		// run the event loop
		_isRunning = true;
		if (!_stateThread.joinable())
			_stateThread = std::thread(&SSLServer::stateChecker, this);

		// start event
		if (_OnStart)
			_OnStart(*this, _port);
	}

	void  SSLServer::stop()
	{
		_isRunning = false;
		if (_stateThread.joinable())
			_stateThread.join();
	}

	void  SSLServer::setTimeout(float t)
	{
		_timeout.tv_sec = t / 1000;
		_timeout.tv_usec = (int)(t * 1000) % (1000 * 1000);
	}

	float SSLServer::getTimeout() const
	{
		return (_timeout.tv_sec * 1000) + ((float)(_timeout.tv_usec) / 1000);
	}

	size_t  SSLServer::getMaxClients() const { return _maxClients; }

	void  SSLServer::OnConnect(std::function < void (Socket::SSLServer &, SSLServer_Client &) > const &callback)
	{
		_OnConnect = callback;
	}

	void  SSLServer::OnDisconnect(std::function < void (Socket::SSLServer &, SSLServer_Client &) > const &callback)
	{
		_OnDisconnect = callback;
	}

	void  SSLServer::OnReadPossible(std::function < void (Socket::SSLServer &, SSLServer_Client &, size_t) > const &callback)
	{
		_OnReadPossible = callback;
	}

	void  SSLServer::OnWritePossible(std::function < void (Socket::SSLServer &, SSLServer_Client &) > const &callback)
	{
		_OnWritePossible = callback;
	}

	void  SSLServer::OnStart(std::function < void (Socket::SSLServer &, int) > const &callback)
	{
		_OnStart = callback;
	}

	void  SSLServer::disconnect(SSLServer_Client &c)
	{
		auto found = std::find(_clients.begin(), _clients.end(), c);

		if (found != std::end(_clients))
		{
			_clients.erase(found);
			closesocket(c.fd);
			FD_CLR(c.fd, &_fd_set);

			auto max = (std::max_element(_clients.begin(), _clients.end(), 
				[](SSLServer_Client const &first, SSLServer_Client const &second) {
					return first.fd < second.fd;
				}));
			if (max != _clients.end())
				_max_fd = (*max).fd;
			else
				_max_fd = _fd;

			if (_OnDisconnect)
				_OnDisconnect(*this, c);
		}
	}

	int  SSLServer::read(SSLServer_Client &c, void *buffer, size_t size)
	{
		// read from the SSL socket
		prefetch_data(c);

		std::cout << "start of read of " << size << " : " << c.internal_read_buffer.size() << std::endl;

		if (c.internal_read_buffer.empty())
			return 0;
		// copy the internal buffer to the given one
		size_t cpy_size = size < c.internal_read_buffer.size() ? size : c.internal_read_buffer.size();
		memcpy(buffer, c.internal_read_buffer.data(), cpy_size);
		// delete copied data from the internal buffer
		auto cpy_buffer = c.internal_read_buffer;
		auto cpy_buffer_iter = cpy_buffer.begin();
		for (int i = 0; i < cpy_size && cpy_buffer_iter != cpy_buffer.end();) {
			++i;
			++cpy_buffer_iter;
		}
		c.internal_read_buffer = std::vector<char>(cpy_buffer_iter, cpy_buffer.end());
		if (c.internal_read_buffer.size() < 4096)
			c.internal_read_buffer.reserve(4096);

		std::cout << "start of read of " << size << " : " << c.internal_read_buffer.size() << std::endl;

		return cpy_size;
	}

	int  SSLServer::write(SSLServer_Client &c, void const *buffer, size_t size)
	{

		int tmp = 0;
		size_t s = 0;

		while (s < size && (tmp = SSL_write(c.ssl, (char *)buffer + s, size - s)) > 0) {
			s += tmp;
		}

		if (tmp < 0)
		{
			if (SSL_get_error(c.ssl, tmp) == SSL_ERROR_WANT_READ ||
				SSL_get_error(c.ssl, tmp) == SSL_ERROR_WANT_WRITE)
				return s;
			else
				disconnect(c);
		}

		return s;
	}

	void SSLServer::prefetch_data(SSLServer_Client &c)
	{
		int tmp = 0;
		char buffer[4096];

		while ((tmp = SSL_read(c.ssl, buffer, 4096)) > 0)
		{
			// page_size optimization : 4096 bytes page
			if ((c.internal_read_buffer.size() + tmp) % 4096 < tmp)
				c.internal_read_buffer.reserve((c.internal_read_buffer.size() / 4096 + 1) * 9096 + 4096);
			// add data to the internal buffer
			c.internal_read_buffer.reserve(c.internal_read_buffer.size() + tmp);
			for (int i = 0; i < tmp; ++i)
				c.internal_read_buffer.push_back(buffer[i]);
		}

		if (tmp < 0)
		{
			// nothing to read anymore
			if (SSL_get_error(c.ssl, tmp) == SSL_ERROR_WANT_READ ||
				SSL_get_error(c.ssl, tmp) == SSL_ERROR_WANT_WRITE)
				return ;
			// error
			else
				disconnect(c);
		}
	}

	size_t SSLServer::bytesAvailables(SSLServer_Client &c)
	{
		prefetch_data(c);
		return c.internal_read_buffer.size();
	}

	void SSLServer::initSSL()
	{
		const SSL_METHOD  *method;

		SSL_library_init();
		OpenSSL_add_all_algorithms();
		ERR_load_BIO_strings();
		ERR_load_crypto_strings();
		SSL_load_error_strings();

		method = SSLv23_server_method();

		_ctx = SSL_CTX_new(method);
		if (!_ctx)
		{
			perror("Unable to create SSL context");
			ERR_print_errors_fp(stderr);
			throw std::exception();
		}

		if (SSL_CTX_use_certificate_file(_ctx, _certPath.c_str(), SSL_FILETYPE_PEM) < 0) {
			ERR_print_errors_fp(stderr);
			throw std::exception();
		}

		if (SSL_CTX_use_PrivateKey_file(_ctx, _pKeyPath.c_str(), SSL_FILETYPE_PEM) < 0 ) {
			ERR_print_errors_fp(stderr);
			throw std::exception();
		}
	}

	bool SSLServer::connect(SSLServer_Client &c)
	{
		std::cout << "connect : " << c.fd << std::endl;
		if (!c.ssl)
		{
			c.ssl = SSL_new(_ctx);
			SSL_set_fd(c.ssl, c.fd);
		}
		if (!c.connected)
		{
			int err = SSL_accept(c.ssl);
			if (err > 0)
			{
				std::cout << c.fd << " connected" << std::endl;
				c.connected = true;
				if (_OnConnect)
					_OnConnect(*this, c);
				return true;
			}
			else
				ERR_print_errors_fp(stderr);
			return false;
		}
	}

	void SSLServer::stateChecker()
	{
		SOCKET fd;
		int nb_fd;
		fd_set read_set, write_set;
		timeval timeout;

		// event loop
		while (_isRunning)
		{
			SLEEPMS(_granularity);
			memcpy(&read_set, &_fd_set, sizeof(_fd_set));
			memcpy(&write_set, &_fd_set, sizeof(_fd_set));

			// wait for an event
			timeout = _timeout;
			nb_fd = select(_max_fd + 1, &read_set, &write_set, NULL, &timeout);
			if (nb_fd < 0 && errno != EINTR)
				throw SocketIOError(std::string(strerror(getError())));

			if (FD_ISSET(_fd, &read_set))
			{
				// connection event
				while ((fd = ::accept(_fd, NULL, NULL)) != -1)
				{
					if (!setNonBloccking(fd))
						throw SocketCreateError(std::string("SSLserver : ") +
							std::string("cannot set non blocking mode for the client fd"));

					FD_SET(fd, &_fd_set);
					_max_fd = (fd > _max_fd) ? fd : _max_fd;
					_clients.push_back(SSLServer_Client(fd));
					connect(_clients.back());
				}
				if (errno != EAGAIN && errno != EWOULDBLOCK)
					throw SocketConnectError(std::string(strerror(getError())));
			}

			for (auto it = _clients.begin(); it != _clients.end() && _clients.size(); ++it)
			{
				// read event
				if (FD_ISSET((*it).fd, &read_set))
				{
					if ((*it).fd == _fd);
					else
					{
						// not SSL connected yet
						if (!(*it).connected)
							connect(*it);
						else
						{
							if (!bytesAvailables(*it))
							{
								disconnect(*it);
								it = _clients.begin();
								continue;
							}
							else if (_OnReadPossible)
								_OnReadPossible(*this, *it, bytesAvailables(*it));
						}
					}
				}
				// write event
				if (FD_ISSET((*it).fd, &write_set))
				{
					if (*it == _fd);
					else
					{
						// not SSL connected yet
						if (!(*it).connected)
							connect(*it);
						else
						{
							if (_OnWritePossible)
								_OnWritePossible(*this, *it);
						}
					}
				}
			}
		}
	}
};
