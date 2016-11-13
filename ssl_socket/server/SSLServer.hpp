//
// Server.hpp for  in /home/agor_m/github/tests_with_cpp_sockets/include
//
// Made by Maxime Agor
// Login   <agor_m@epitech.net>
//
// Started on  Mon May  2 15:20:47 2016 Maxime Agor
// Last update Sun Jun  5 18:54:18 2016 Maxime Agor
//

#ifndef SSLSERVER_HPP_
# define SSLSERVER_HPP_

#include <functional>
# include <thread>
# include <atomic>
# include <sys/time.h>
# include <vector>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>


# include "Sockets.hpp"

namespace Socket
{

	struct SSLServer_Client
	{
		public:
			SOCKET fd;
			SSL *ssl;
			std::vector<char> internal_read_buffer;
			bool connected;

			SSLServer_Client(SOCKET fd_)
			{
				ssl = NULL;
				fd = fd_;
				internal_read_buffer.reserve(4096);
				connected = false;
			}

			SSLServer_Client(SSLServer_Client const &oth)
			{
				fd = oth.fd;
				internal_read_buffer = std::vector<char>(oth.internal_read_buffer);
				internal_read_buffer.reserve((internal_read_buffer.size() / 4096 + 1) * 4096);
				connected = oth.connected;
				ssl = oth.ssl;
			}

			bool operator==(SSLServer_Client const &oth) const
			{
				return oth.fd == fd &&
                       oth.connected == connected &&
                       oth.ssl == ssl;
			}
	};

	class SSLServer
	{
	public:
		SSLServer(std::string certificate = "server.crt", std::string privateKey = "server.crt");
		virtual ~SSLServer();

		/* SSLSERVER CONFIGURATION API */

		void start(int port, size_t maxClients = 1);
		void stop();

		// set the timeout in ms
		void     setTimeout(float t);
		float    getTimeout() const;

		// set the event loop granularity in MS (default 1MS)
		void    setGranularity(uint g) {_granularity = g;}
		uint    getGranularity() const {return _granularity;}

		size_t   getMaxClients() const;

		/* CALLBACKS API */
		// param0 : *this

		// called on client connexion
		// param1 : Int : fd of the client socket
		void  OnConnect(std::function < void (SSLServer &, SSLServer_Client &) > const &callback);
		// called on client disconnexion
		// param1 : Int : closed fd of the client socket
		void  OnDisconnect(std::function < void (SSLServer &, SSLServer_Client &) > const &callback);
		// called on client read accessibility
		// param1 : Int : fd of the client socket
		// param2 : size_t : size possible to read (can change meanwhile)
		void  OnReadPossible(std::function < void (SSLServer &, SSLServer_Client &, size_t) > const &callback);
		// called on client write accessibility
		// param1 : Int : fd of the client socket
		void  OnWritePossible(std::function < void (SSLServer &, SSLServer_Client &) > const &callback);
		// called on SSLserver startup
		// param1 : Int : Port of the connexion
		void  OnStart(std::function < void (SSLServer &, int) > const &callback);


		/* I/O API */
		void    disconnect(SSLServer_Client &c);
		int     read(SSLServer_Client &c, void *buffer, size_t size);
		int     write(SSLServer_Client &c, void const *buffer, size_t size);
		size_t  bytesAvailables(SSLServer_Client &c);

	protected:
		int                                             _port;
		size_t                                          _maxClients;
		SOCKET                                          _fd;
		std::vector<SSLServer_Client>                   _clients;
		std::thread                                     _stateThread;
		timeval                                         _timeout;
		uint											_granularity;

		std::function < void (SSLServer &, SSLServer_Client &) >          _OnConnect;
		std::function < void (SSLServer &, SSLServer_Client &) >          _OnDisconnect;
		std::function < void (SSLServer &, SSLServer_Client &, size_t) >  _OnReadPossible;
		std::function < void (SSLServer &, SSLServer_Client &) >          _OnWritePossible;
		std::function < void (SSLServer &, int) >          _OnStart;

		std::string _certPath;
		std::string _pKeyPath;

		SSL_CTX           *_ctx;


		bool connect(SSLServer_Client &);
		void onDisconnect(Socket::SSLServer &serv, Socket::SSLServer_Client &c);
		void prefetch_data(SSLServer_Client &c);

	private:
		void  stateChecker();
		void  initSSL();
		fd_set                                          _fd_set;
		int                                             _max_fd;
		std::atomic<bool>                               _isRunning;
	};
};

#endif
