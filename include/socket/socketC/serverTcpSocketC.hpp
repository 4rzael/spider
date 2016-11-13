//
// serverTcpSocket.hpp for  in /home/gandoulf/epitech/cpp_spider/include/socket
//
// Made by Gandoulf
// Login   <gandoulf@epitech.net>
//
// Started on  Sat Nov  5 12:20:28 2016 Gandoulf
// Last update Sat Nov 12 14:53:12 2016 Gandoulf
//

#ifndef SERVERTCPSOCKETC_HPP_
# define SERVERTCPSOCKETC_HPP_

#include "spider/packetUnserializer.hpp"
#include "spider/packetSerializer.hpp"
#include "bdd_connect/SqlServer.hh"
#include "Network/sockets/Server.hpp"

#include <set>
#include <memory>
#include <thread>
#include <map>

namespace spider
{
  namespace socket
  {
    class user;
    typedef std::shared_ptr<user> user_ptr;

    //class holding connected client
    class user : public std::enable_shared_from_this<user>
    {
    public:
      user(Socket::Server & server, std::set<user_ptr> & clients, SqlServer &sqlServer, int fd);
      user_ptr start();
      void close();
      void read();

      template<class packet>
      void write(spider::PacketSerializer<packet> data)
      {
	_messages.push_back(data.getPackedData(1));
	_messagesSize.push_back(data.getPacketSize());
      }
      void doWrite();

    private:
      void readHeader();
      void readData();

    private:
      Socket::Server			&_server;
      int				_fd;
      std::set<user_ptr>		&_clients;
      SqlServer				&_sqlServer;

      //packet
      std::list<char *>			_messages;
      std::list<int>			_messagesSize;
      spider::PacketUnserializer	_packet;
      char				_data[128];
    };

    //class for the server behavior
    class ServerTcpSocket
    {
    public:
      ServerTcpSocket(int port, size_t maxClient = 1000);
      void close();
      void startService();
      void closeService();

      template<class packet>
      void write(spider::PacketSerializer<packet> data)
      {
	for (auto ite = _clients.begin(); ite != _clients.end(); ++ite)
	  (*ite).get()->write(data);
      }

    private:
      user_ptr accept(int fd);

    private:
      Socket::Server			_server;
      std::map<int, user_ptr>		_clientsFD;
      bool				_runningService;
      std::set<user_ptr>		_clients;
      SqlServer				_sqlServer;

      //server
      int				_port;
      size_t				_maxClient;
    };
  }
}

#endif /* SERVERTCPSOCKETC_HPP_ */
