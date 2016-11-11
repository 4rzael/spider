//
// serverTcpSocket.hpp for  in /home/gandoulf/epitech/cpp_spider/include/socket
//
// Made by Gandoulf
// Login   <gandoulf@epitech.net>
//
// Started on  Sat Nov  5 12:20:28 2016 Gandoulf
// Last update Fri Nov 11 15:45:27 2016 Gandoulf
//

#ifndef SERVERTCPSOCKET_HPP_
# define SERVERTCPSOCKET_HPP_

#include "spider/packetUnserializer.hpp"
#include "spider/packetSerializer.hpp"
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
	_messages.push_back(data.getPackedData());
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
				 boost::asio::buffer(_messages.front().get(),
						     _messagesSize.front()),
				 [this](boost::system::error_code ec, std::size_t)
				 {
				   if (!ec)
				     {
				       std::cout << "packet send" << std::endl;
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
      std::list<spider::packedData>	_messages;
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
	return;
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
