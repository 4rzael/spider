//
// main.cpp for  in /home/gandoulf/epitech/cpp_spider
//
// Made by Gandoulf
// Login   <gandoulf@epitech.net>
//
// Started on  Tue Oct 25 16:46:11 2016 Gandoulf
// Last update Mon Nov 14 00:01:57 2016 debrab_t
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
	  PackageCMDLogOut msg;
	  //memset(&msg.str[0], 0, 100);
	  std::strncpy(msg.cmd, "DEC", 3);
	  //std::memcpy(msg.str, line, strlen(line));
	  spider::PacketSerializer<PackageCMDLogOut> packet(sizeof(PackageHeader) + sizeof(Test), 1999888256, msg);
	  server.write<PackageCMDLogOut>(packet);
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
	//c.startService();

	char line[128 + 1];
	// Connect client
	    PackageCMDIDN msg1;
	    memset(&msg1.key[0], 0, 100);
	    std::strncpy(msg1.cmd, "IDN", 3);
	    std::memcpy(msg1.key, line, strlen(line));
	    spider::PacketSerializer<PackageCMDIDN> packet1(sizeof(PackageHeader) + sizeof(PackageCMDIDN), 4, msg1);
	    c.write<PackageCMDIDN>(packet1);


	while (std::cin.getline(line, 128 + 1)) {
	  if (!std::strncmp(line, "quit", 4))
	    break;
	  /* Dec
	  PackageCMDLogOut msg;
	  std::strncpy(msg.cmd, "dec", 3);
	  //std::memcpy(msg.key, line, strlen(line));
	  spider::PacketSerializer<PackageCMDLogOut> packet(sizeof(PackageHeader) + sizeof(PackageCMDLogOut), 4, msg);
	  c.write<PackageCMDLogOut>(packet);
	  */
	  // keyboard

	  PackageCMDKeyboardTouch msg;
	  memset(&msg.id[0], 0, 50);
	  std::memcpy(msg.id, line, strlen(line));
	  msg.timestamp = 5;
	  std::strncpy(msg.cmd, "TOC", 3);
	  //std::memcpy(msg.key, line, strlen(line));
	  spider::PacketSerializer<PackageCMDKeyboardTouch> packet2(sizeof(PackageHeader) + sizeof(PackageCMDKeyboardTouch), 4, msg);
	  c.write<PackageCMDKeyboardTouch>(packet2);

	  //mouse move
	  /*
	  PackageCMDMouseMove msg2;
	  msg2.x = 1;
	  msg2.y = 2;
	  msg2.timestamp = 5;
	  std::strncpy(msg2.cmd, "mVt", 3);
	  //std::memcpy(msg.key, line, strlen(line));
	  spider::PacketSerializer<PackageCMDMouseMove> packet3(sizeof(PackageHeader) + sizeof(PackageCMDMouseMove), 4, msg2);

	  c.write<PackageCMDMouseMove>(packet3);
	  */
	  // mouse click
	  /*
	  PackageCMDMouseClic msg3;
	  msg3.x = 1;
	  msg3.y = 2;
	  msg3.id[0] = 'a';
	  msg3.id[1] = '\0';
	  std::strncpy(msg3.cmd, "clc", 3);
	  //std::memcpy(msg.key, line, strlen(line));
	  spider::PacketSerializer<PackageCMDMouseClic> packet4(sizeof(PackageHeader) + sizeof(PackageCMDMouseClic), 4, msg3);
	  c.write<PackageCMDMouseClic>(packet4);
	  */
	  /*
	    // Connect client
	    PackageCMDIDN msg;
	    memset(&msg.key[0], 0, 100);
	    std::strncpy(msg.cmd, "idn", 3);
	    std::memcpy(msg.key, line, strlen(line));
	    spider::PacketSerializer<PackageCMDIDN> packet(sizeof(PackageHeader) + sizeof(PackageCMDIDN), 4, msg);
	    c.write<PackageCMDIDN>(packet);
	  */
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
