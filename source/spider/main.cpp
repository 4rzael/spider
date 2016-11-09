//
// main.cpp for  in /home/gandoulf/epitech/cpp_spider
//
// Made by Gandoulf
// Login   <gandoulf@epitech.net>
//
// Started on  Tue Oct 25 16:46:11 2016 Gandoulf
// Last update Wed Nov  9 15:33:15 2016 debrab_t
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
	  PackageCMDIDN msg;
	  memset(&msg.key[0], 0, 100);
	  std::strncpy(msg.cmd, "idn", 3);
	  std::memcpy(msg.key, line, strlen(line));
	  spider::PacketSerializer<PackageCMDIDN> packet(sizeof(PackageHeader) + sizeof(PackageCMDIDN), 1999888256, msg);
	  c.write<PackageCMDIDN>(packet);
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
