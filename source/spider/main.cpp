//
// main.cpp for  in /home/gandoulf/epitech/cpp_spider
//
// Made by Gandoulf
// Login   <gandoulf@epitech.net>
//
// Started on  Tue Oct 25 16:46:11 2016 Gandoulf
// Last update Sat Nov 12 17:59:44 2016 Gandoulf
//

//#include "socket/clientTcpSocket.hpp"
//#include "socket/serverTcpSocket.hpp"
#include "socket/socketC/clientTcpSocketC.hpp"
#include "socket/socketC/serverTcpSocketC.hpp"
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
	ServerTcpSocket server(atoi(av[1]));
	server.startService();
	char line[128 + 1];
	while (std::cin.getline(line, 128 + 1)) {
	  if (!std::strncmp(line, "quit", 4))
	    {
	      server.close();
	      break;
	    }
	  Test msg;
	  memset(&msg.str[0], 0, 100);
	  std::strncpy(msg.cmd, "tst", 3);
	  std::memcpy(msg.str, line, strlen(line));
	  spider::PacketSerializer<Test> packet(sizeof(PackageHeader) + sizeof(Test), 1999888256, msg);
	  server.write<Test>(packet);
	}
	server.closeService();
      }
      catch (std::exception& e) {
	std::cerr << "Exception: " << e.what() << "\n";
      }
    }
  else if (ac == 3)
    {
      try {
	ClientTcpSocket c(std::string(av[1]), atoi(av[2]));
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
