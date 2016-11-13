#include "SSLClient.hpp"
#include <string>
#include <queue>
#include <cstdlib>
#include <algorithm>
#include <iostream>

std::queue<std::string> _writeBuffer;
Socket::SSLClient _tcp;

void	start(std::string address, int port);
void	stop();
void  onDisconnect(Socket::SSLClient &c);
void  onReadPossible(Socket::SSLClient &c, size_t size);
void  onWritePossible(Socket::SSLClient &c);


int main(int ac, char **av) {
	if (ac < 3)
	{
		std::cout << "usage : " << av[0] << " HOST PORT" << std::endl;
		return 1;
	}
	start(std::string(av[1]), atoi(av[2]));
	while (1)
		SLEEPMS(1000);
}


void	start(std::string address, int port)
{
	std::cout << "start" << std::endl;
	_tcp.OnDisconnect(
		[](Socket::SSLClient &c)
		{
			onDisconnect(c);
		});
	_tcp.OnReadPossible(
		[](Socket::SSLClient &c, size_t size)
		{
			onReadPossible(c, size);
		});
	_tcp.OnWritePossible(
		[](Socket::SSLClient &c)
		{
			onWritePossible(c);
		});

	_writeBuffer.push("coucou");

	_tcp.start(address, port);
	std::cout << "end start" << std::endl;
}

void	stop()
{
	_tcp.stop();
}

// TCP callbacks

void  onDisconnect(Socket::SSLClient &c)
{
	std::cout << "connexion interrupted" << std::endl;
}

// read, then write back what you got
void  onReadPossible(Socket::SSLClient &c, size_t size)
{
	char *tmp;

	std::cout << "can read " << size << " bytes" << std::endl;

	tmp = new char[size + 1];
	bzero(tmp, size + 1);
	c.read(tmp, size);
	std::cout << tmp << std::endl;
	_writeBuffer.push(std::string(tmp));
	delete[] tmp;
}

void  onWritePossible(Socket::SSLClient &c)
{
	if (!_writeBuffer.size())
		return;

	std::string tmp;

	tmp = _writeBuffer.front();
	_writeBuffer.pop();
	c.write(tmp.c_str(), tmp.length());
}
