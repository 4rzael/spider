//
// main.cpp for  in /home/gandoulf/epitech/cpp_spider
//
// Made by Gandoulf
// Login   <gandoulf@epitech.net>
//
// Started on  Tue Oct 25 16:46:11 2016 Gandoulf
// Last update Mon Nov  7 15:05:16 2016 Gandoulf
//

#include "socket/clientTcpSocket.hpp"
#include "socket/serverTcpSocket.hpp"
#include "spider/spider.h"

#include <cstdlib>
#include <list>
#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;
using spider::socket::ServerTcpSocket;
using spider::socket::ClientTcpSocket;

int	main(int ac, char **av)
{
  if (ac == 2)
    {
      try {
	boost::asio::io_service io_service;

	std::list<ServerTcpSocket> servers;
	for (int i = 1; i < ac; ++i) {
	  tcp::endpoint endpoint(tcp::v4(), std::atoi(av[i]));
	  servers.emplace_back(io_service, endpoint);
	}
	io_service.run();
      }
      catch (std::exception& e) {
	std::cerr << "Exception: " << e.what() << "\n";
      }
    }
  else if (ac == 3)
    {
      try {
	boost::asio::io_service io_service;
	tcp::resolver resolver(io_service);
	auto endpoint_iterator = resolver.resolve({ av[1], av[2] });

	ClientTcpSocket c(io_service, endpoint_iterator);
	c.connect();
	c.startService();

	char line[128 + 1];
	while (std::cin.getline(line, 128 + 1)) {
	  if (!std::strncmp(line, "quit", 4))
	    break;
	  Test msg;
	  memset(&msg.str[0], 0, 100);
	  std::strncpy(msg.cmd, "tst", 3);
	  std::memcpy(msg.str, line, strlen(line));
	  spider::PacketSerializer<Test> packet(sizeof(PackageHeader) + sizeof(Test), 1999888256, msg);
	  c.write<Test>(packet);
	}
	c.close();
	c.closeService();
      }
      catch (std::exception& e)	{
	std::cerr << "Exception: " << e.what() << "\n";
      }
    }
  return 0;
}
