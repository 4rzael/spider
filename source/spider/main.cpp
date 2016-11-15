//
// main.cpp for  in /home/gandoulf/epitech/cpp_spider
//
// Made by Gandoulf
// Login   <gandoulf@epitech.net>
//
// Started on  Tue Oct 25 16:46:11 2016 Gandoulf
// Last update Tue Nov 15 13:29:35 2016 debrab_t
//

//#include "socket/clientTcpSocket.hpp"
//#include "socket/serverTcpSocket.hpp"
#include "socket/socketC/clientTcpSocketC.hpp"
#include "socket/socketC/serverTcpSocketC.hpp"
#include "spider/handleCmdToClient.hh"
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
	HandleCmdToClient	cmdTC;
	ServerTcpSocket		server(atoi(av[1]));
	server.startService();
	char line[100];
	while (std::cin.getline(line, 100)) {
	  if (!std::strncmp(line, "quit", 4))
	    {
	      server.close();
	      break;
	    }
	  cmdTC.sendCmdToClient(line, server);
	}
	server.closeService();
      }
      catch (std::exception& e) {
	std::cerr << "Exception: " << e.what() << "\n";
      }
    }
  return 0;
}
