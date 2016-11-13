#include <errno.h>
#include <algorithm>
#include <utility>

#ifndef _WIN32
# include <sys/select.h>
#endif

#include <iostream>
#include <fcntl.h>

#include "Network/sockets/Server.hpp"

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

	Server::Server(std::string certificate, std::string privateKey)
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

	Server::~Server()
	{
	}

	void  Server::start(int port, size_t maxClients)
	{
		SOCKADDR_IN	addr;
		PROTOENT	*proto;

		// port checking
		if (port <= 0 || port > 0xFFFF)
			throw SocketCreateError(std::string("server : bad port"));
		_port = port;
		_maxClients = maxClients;

		initSSL();

		// create socket
		proto = getprotobyname("tcp");

		if ((_fd = socket(AF_INET, SOCK_STREAM, proto ? proto->p_proto : 0)) == -1)
			throw SocketCreateError(std::string("server : ") +
				std::string(strerror(getError())));

		if (!setNonBloccking(_fd))
			throw SocketCreateError(std::string("server : ") +
				std::string("cannot set non blocking mode for the server fd"));

		// bind
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port = htons(port);

		if (bind(_fd, (sockaddr *)(&addr), sizeof(addr)) == -1)
			throw SocketConnectError("server : " + std::string(strerror(getError())));
		if (listen(_fd, _maxClients) == -1)
			throw SocketConnectError("server : " + std::string(strerror(getError())));

		// create select() sets
		FD_ZERO(&_fd_set);
		_max_fd = _fd;
		FD_SET(_fd, &_fd_set);

		// run the event loop
		_isRunning = true;
		if (!_stateThread.joinable())
			_stateThread = std::thread(&Server::stateChecker, this);

		// start event
		if (_OnStart)
			_OnStart(*this, _port);
	}

	void  Server::stop()
	{
		_isRunning = false;
		if (_stateThread.joinable())
			_stateThread.join();
	}

	void  Server::setTimeout(float t)
	{
		_timeout.tv_sec = t / 1000;
		_timeout.tv_usec = (int)(t * 1000) % (1000 * 1000);
	}

	float Server::getTimeout() const
	{
		return (_timeout.tv_sec * 1000) + ((float)(_timeout.tv_usec) / 1000);
	}

	size_t  Server::getMaxClients() const { return _maxClients; }

	void  Server::OnConnect(std::function < void (Socket::Server &, int) > const &callback)
	{
		_OnConnect = callback;
	}

	void  Server::OnDisconnect(std::function < void (Socket::Server &, int) > const &callback)
	{
		_OnDisconnect = callback;
	}

	void  Server::OnReadPossible(std::function < void (Socket::Server &, int, size_t) > const &callback)
	{
		_OnReadPossible = callback;
	}

	void  Server::OnWritePossible(std::function < void (Socket::Server &, int) > const &callback)
	{
		_OnWritePossible = callback;
	}

	void  Server::OnStart(std::function < void (Socket::Server &, int) > const &callback)
	{
		_OnStart = callback;
	}

	void  Server::disconnect(int fd)
	{

		if (!isConnected(fd))
		{
			return ;
		}

		auto found = _clients.find(fd);
		if (found != std::end(_clients))
		{
			_clients.erase(found);
			closesocket(fd);
			FD_CLR(fd, &_fd_set);

			auto max = (std::max_element(_clients.begin(), _clients.end(), 
				[]( std::pair<int, Server_Client> const &first,
					std::pair<int, Server_Client> const &second) {
					return std::get<1>(first).fd < std::get<1>(second).fd;
				}));
			if (max != _clients.end())
				_max_fd = std::get<1>(*max).fd;
			else {
				FD_ZERO(&_fd_set);
				_max_fd = _fd;
				FD_SET(_fd, &_fd_set);
			}

			if (_OnDisconnect)
				_OnDisconnect(*this, fd);
		}
	}

	int  Server::read(int fd, void *buffer, size_t size)
	{

		if (!isConnected(fd))
			return 0;

		Server_Client &c = getClientByFd(fd);

		// read from the SSL socket
		prefetch_data(fd);


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

		return cpy_size;
	}

	int  Server::write(int fd, void const *buffer, size_t size)
	{

		if (!isConnected(fd))
			return 0;

		Server_Client &c = getClientByFd(fd);

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
				disconnect(fd);
		}

		return s;
	}

	void Server::prefetch_data(int fd)
	{

		if (!isConnected(fd))
			return ;

		Server_Client &c = getClientByFd(fd);

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
				disconnect(fd);
		}
	}

	size_t Server::bytesAvailables(int fd)
	{

		if (!isConnected(fd))
			return 0;

		prefetch_data(fd);

		Server_Client &c = getClientByFd(fd);

		return c.internal_read_buffer.size();
	}

	void Server::initSSL()
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

	bool Server::makeHandshake(int fd)
	{
		if (!isConnected(fd))
			return false;

		Server_Client &c = getClientByFd(fd);

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
				c.connected = true; // SSL authentified
				if (_OnConnect)
					_OnConnect(*this, fd);
				return true;
			}
			else
				ERR_print_errors_fp(stderr);
			return false;
		}
	}

	bool Server::isConnected(int fd) const
	{

		auto found = _clients.find(fd);
		return found != _clients.end();
	}

	Server_Client &Server::getClientByFd(int fd)
	{

		return _clients.at(fd);
	}

	void Server::stateChecker()
	{

		SOCKET fd;
		int nb_fd;
		fd_set read_set, write_set;
		timeval timeout;

		// event loop
		while (_isRunning)
		{
			SLEEPMS(_granularity);
			FD_SET(_fd, &_fd_set);
			memcpy(&read_set, &_fd_set, sizeof(_fd_set));
			memcpy(&write_set, &_fd_set, sizeof(_fd_set));

			for (auto c = _clients.begin(); c != _clients.end(); ++c) {
			}

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
						throw SocketCreateError(std::string("server : ") +
							std::string("cannot set non blocking mode for the client fd"));

					FD_SET(fd, &_fd_set);
					_max_fd = (fd > _max_fd) ? fd : _max_fd;
					_clients.emplace(fd, Server_Client(fd));
					makeHandshake(fd);
				}
				if (errno != EAGAIN && errno != EWOULDBLOCK)
					throw SocketConnectError(std::string(strerror(getError())));
			}

			for (auto it = _clients.begin(); it != _clients.end() && _clients.size(); ++it)
			{

				// read event
				if (FD_ISSET(std::get<1>(*it).fd, &read_set))
				{
					if (std::get<1>(*it).fd == _fd);
					else
					{
						// not SSL connected yet
						if (!std::get<1>(*it).connected)
							makeHandshake(std::get<0>(*it));
						else
						{
							if (!bytesAvailables(std::get<0>(*it)))
							{
								disconnect(std::get<0>(*it));
								if (_clients.size() == 0)
									break;
								it = _clients.begin();
								continue;
							}
							else if (_OnReadPossible && isConnected(std::get<0>(*it)))
								_OnReadPossible(*this, std::get<0>(*it), bytesAvailables(std::get<0>(*it)));
						}
					}
				}
				// write event
				if (FD_ISSET(std::get<1>(*it).fd, &write_set))
				{
					if (std::get<0>(*it) == _fd);
					else
					{
						// not SSL connected yet
						if (!std::get<1>(*it).connected)
							makeHandshake(std::get<0>(*it));
						else
						{
							if (_OnWritePossible && isConnected(std::get<0>(*it)))
								_OnWritePossible(*this, std::get<0>(*it));
						}
					}
				}
			}
		}
	}
};
