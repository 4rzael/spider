//
// main.cpp for  in /home/gandoulf/epitech/cpp_spider
//
// Made by Gandoulf
// Login   <gandoulf@epitech.net>
//
// Started on  Tue Oct 25 16:46:11 2016 Gandoulf
// Last update Mon Nov 14 18:51:20 2016 debrab_t
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
  else if (ac == 4)
    {
      try {
	ClientTcpSocket c(std::string(av[1]), atoi(av[2]));
	c.connect();
	//c.startService();
	int id = atoi(av[3]);

	char line[128 + 1];
	PackageCMDIDN msg1;
	memset(&msg1.key[0], 0, 100);
	std::strncpy(msg1.cmd, "IDN", 3);
	std::memcpy(msg1.key, line, strlen(line));
	spider::PacketSerializer<PackageCMDIDN> packet1(sizeof(PackageHeader) + sizeof(PackageCMDIDN), id, msg1);
	c.write<PackageCMDIDN>(packet1);

	while (std::cin.getline(line, 128 + 1)) {
	  if (!std::strncmp(line, "quit", 4))
	    break;
	  PackageCMDKeyboardTouch msg;
	  memset(&msg.id[0], 0, 30);
	  std::strncpy(msg.cmd, "TOC", 3);
	  std::memcpy(msg.id, line, strlen(line));
	  spider::PacketSerializer<PackageCMDKeyboardTouch> packet(sizeof(PackageHeader) + sizeof(PackageCMDKeyboardTouch), id, msg);
	  c.write<PackageCMDKeyboardTouch>(packet);
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
