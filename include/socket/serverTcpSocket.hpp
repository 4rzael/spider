//
// serverTcpSocket.hpp for  in /home/gandoulf/epitech/cpp_spider/include/socket
//
// Made by Gandoulf
// Login   <gandoulf@epitech.net>
//
// Started on  Sat Nov  5 12:20:28 2016 Gandoulf
// Last update Sun Nov 13 14:48:02 2016 debrab_t
//

#ifndef SERVERTCPSOCKET_HPP_
# define SERVERTCPSOCKET_HPP_

#include "spider/packetUnserializer.hpp"
#include "spider/packetSerializer.hpp"
#include "bdd_connect/SqlManager.hh"
#include "bdd_connect/SqlServer.hh"

#include <set>
#include <memory>
#include <boost/asio.hpp>
#include <thread>

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
      user(boost::asio::ip::tcp::socket socket, std::set<user_ptr> & clients,
	   SqlServer &sqlServer);
      void start();
      void close();

      template<class packet>
      void write(spider::PacketSerializer<packet> data)
      {
	_messages.push_back(data.getPackedData(1));
	_messagesSize.push_back(data.getPacketSize());
	if (!_writing)
	  doWrite();
      }

    private:
      void readHeader();
      void readData();
      void doWrite()
      {
	_writing = true;
	boost::asio::async_write(_socket,
				 boost::asio::buffer(_messages.front(),
						     _messagesSize.front()),
				 [this](boost::system::error_code ec, std::size_t)
				 {
				   if (!ec)
				     {
				       std::cout << "packet send" << std::endl;
				       delete[] _messages.front();
				       _messages.pop_front();
				       _messagesSize.pop_front();
				       if (!_messages.empty())
					 doWrite();
				       else
					 _writing = false;
				     }
				   else
				     {
				       _socket.close();
				     }
				 });
      }

    private:
      boost::asio::ip::tcp::socket	_socket;
      std::set<user_ptr>		&_clients;
      SqlServer				&_sqlServer;

      //packet
      std::list<char *>			_messages;
      std::list<int>			_messagesSize;
      bool				_writing;
      spider::PacketUnserializer	_packet;
      char				_data[128];
    };

    //class for the server behavior
    class ServerTcpSocket
    {
    public:
      ServerTcpSocket(boost::asio::io_service& io_service,
	     const boost::asio::ip::tcp::endpoint& endpoint);
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
      void accept();

    private:
      boost::asio::io_service &		_ioService;
      boost::asio::ip::tcp::acceptor	_acceptor;
      boost::asio::ip::tcp::socket	_socket;
      std::shared_ptr<std::thread>	_runningService;
      std::set<user_ptr>		_clients;
      SqlServer				_sqlServer;
    };
  }
}

#endif /* SERVERTCPSOCKET_HPP_ */
