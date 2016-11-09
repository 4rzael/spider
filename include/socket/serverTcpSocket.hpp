//
// serverTcpSocket.hpp for  in /home/gandoulf/epitech/cpp_spider/include/socket
//
// Made by Gandoulf
// Login   <gandoulf@epitech.net>
//
// Started on  Sat Nov  5 12:20:28 2016 Gandoulf
// Last update Wed Nov  9 14:34:32 2016 debrab_t
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
	doWrite(data);
      }

    private:
      void readHeader();
      void readData();
      template<class packet>void
      doWrite(spider::PacketSerializer<packet> data)
      {
	char *packet;

	packet = data.getPackedData();
	boost::asio::async_write(_socket,
				 boost::asio::buffer(packet, data.getPacketSize()),
				 [this, packet](boost::system::error_code ec, std::size_t)
				 {
				   if (!ec)
				     {
				       std::cout << "packet send" << std::endl;
				     }
				   else
				     {
				       _socket.close();
				     }
				   delete[] packet;
				 });packet = data.getPackedData();
      }

    private:
      boost::asio::ip::tcp::socket	_socket;
      spider::PacketUnserializer	_packet;
      char				_data[128];
      std::set<user_ptr>		&_clients;
      SqlServer				&_sqlServer;
    };

    //class for the server behavior
    class ServerTcpSocket
    {
    public:
      ServerTcpSocket(boost::asio::io_service& io_service,
	     const boost::asio::ip::tcp::endpoint& endpoint);

    private:
      void accept();

    private:
      boost::asio::ip::tcp::acceptor	_acceptor;
      boost::asio::ip::tcp::socket	_socket;
      std::set<user_ptr>		_clients;
      SqlServer				_sqlServer;
    };
  }
}

#endif /* SERVERTCPSOCKET_HPP_ */
