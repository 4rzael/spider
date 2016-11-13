#include "SSLServer.hpp"
#include <string>
#include <queue>
#include <cstdlib>
#include <algorithm>
#include <iostream>

std::queue<std::string> _writeBuffer;
Socket::SSLServer _tcp("../spider.crt", "../spider.key");

void start(int port);
void stop();
void onConnect(Socket::SSLServer &serv, Socket::SSLServer_Client &c);
void onDisconnect(Socket::SSLServer &serv, Socket::SSLServer_Client &c);
void onReadPossible(Socket::SSLServer &serv, Socket::SSLServer_Client &c, size_t size);
void onWritePossible(Socket::SSLServer &serv, Socket::SSLServer_Client &c);


int main(int ac, char **av) {
	if (ac < 2)
	{
		std::cout << "usage : " << av[0] << " PORT" << std::endl;
		return 1;
	}
	start(atoi(av[1]));
	while (1)
		SLEEPMS(1000);
}


void	start(int port)
{
	std::cout << "start" << std::endl;
	_tcp.OnConnect(
		[](Socket::SSLServer &serv, Socket::SSLServer_Client &c)
		{
			onConnect(serv, c);
		});
	_tcp.OnDisconnect(
		[](Socket::SSLServer &serv, Socket::SSLServer_Client &c)
		{
			onDisconnect(serv, c);
		});
	_tcp.OnReadPossible(
		[](Socket::SSLServer &serv, Socket::SSLServer_Client &c, size_t size)
		{
			onReadPossible(serv, c, size);
		});
	_tcp.OnWritePossible(
		[](Socket::SSLServer &serv, Socket::SSLServer_Client &c)
		{
			onWritePossible(serv, c);
		});

	_tcp.start(port);
	std::cout << "end start" << std::endl;
}

void	stop()
{
	_tcp.stop();
}

// TCP callbacks

void onConnect(Socket::SSLServer &serv, Socket::SSLServer_Client &c)
{
	std::cout << "client connected" << std::endl;
	_writeBuffer.push("A message from the server");
}

void  onDisconnect(Socket::SSLServer &serv, Socket::SSLServer_Client &c)
{
	std::cout << "connexion interrupted" << std::endl;
}

// read, then write back what you got
void  onReadPossible(Socket::SSLServer &serv, Socket::SSLServer_Client &c, size_t size)
{
	char *tmp;

	std::cout << "can read " << size << " bytes" << std::endl;

	tmp = new char[size + 1];
	bzero(tmp, size + 1);
	serv.read(c, tmp, size);
	std::cout << "message from client : " << tmp << std::endl;
	delete[] tmp;
}

void  onWritePossible(Socket::SSLServer &serv, Socket::SSLServer_Client &c)
{
	if (!_writeBuffer.size())
		return;

	std::string tmp;


	tmp = _writeBuffer.front();
	_writeBuffer.pop();
	std::cout << "writing " << tmp << std::endl;

	serv.write(c, tmp.c_str(), tmp.length());
}
