//
// serverTcpSocket.hpp for  in /home/gandoulf/epitech/cpp_spider/include/socket
//
// Made by Gandoulf
// Login   <gandoulf@epitech.net>
//
// Started on  Sat Nov  5 12:20:28 2016 Gandoulf
// Last update Sun Nov 13 20:47:00 2016 Gandoulf
//

#ifndef SERVERTCPSOCKETC_HPP_
# define SERVERTCPSOCKETC_HPP_

#include "spider/packetUnserializer.hpp"
#include "spider/packetSerializer.hpp"
#include "bdd_connect/SqlServer.hh"
#include "Network/sockets/Server.hpp"

#include <set>
#include <memory>
#include <map>
#include <mutex>

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
      user(Socket::Server & server, std::set<user_ptr> & clients, SqlServer &sqlServer,
	   Socket::Server_Client &c, std::mutex & Mclients);
      user_ptr start();
      void disconnect();
      void close();
      void read(size_t length);

      template<class packet>
      void write(spider::PacketSerializer<packet> data)
      {
	_Mqueue.lock();
	_messages.push_back(data.getPackedData(1));
	_messagesSize.push_back(data.getPacketSize());
	_Mqueue.unlock();
      }
      void doWrite();

    private:
      void readHeader();
      void readData();

    private:
      Socket::Server			&_server;
      bool				_firstRead;
      Socket::Server_Client		&_fd;
      std::set<user_ptr>		&_clients;
      SqlServer				&_sqlServer;
      std::mutex			&_Mclients;

      //packet
      std::list<char *>			_messages;
      std::list<int>			_messagesSize;
      spider::PacketUnserializer	_packet;
      char				_data[128];
      std::mutex			_Mqueue;
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
      user_ptr accept(Socket::Server_Client &c);

    private:
      Socket::Server			_server;
      std::map<Socket::Server_Client , user_ptr>		_clientsFD;
      bool				_runningService;
      std::set<user_ptr>		_clients;
      SqlServer				_sqlServer;
      std::mutex			_Mclients;

      //server
      int				_port;
      size_t				_maxClient;
    };
  }
}

#endif /* SERVERTCPSOCKETC_HPP_ */
