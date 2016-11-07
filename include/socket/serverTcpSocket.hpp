//
// serverTcpSocket.hpp for  in /home/gandoulf/epitech/cpp_spider/include/socket
//
// Made by Gandoulf
// Login   <gandoulf@epitech.net>
//
// Started on  Sat Nov  5 12:20:28 2016 Gandoulf
// Last update Mon Nov  7 14:24:36 2016 Gandoulf
//

#ifndef SERVERTCPSOCKET_HPP_
# define SERVERTCPSOCKET_HPP_

#include "spider/packetUnserializer.hpp"

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
      user(boost::asio::ip::tcp::socket socket, std::set<user_ptr> & clients);
      void start();
      void close();

    private:
      void readHeader();
      void readData();

    private:
      boost::asio::ip::tcp::socket	_socket;
      spider::PacketUnserializer	_packet;
      char				_data[128];
      std::set<user_ptr>		&_clients;
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
    };
  }
}

#endif /* SERVERTCPSOCKET_HPP_ */
