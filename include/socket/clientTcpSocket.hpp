//
// clientTcpSocket.hpp for  in /home/gandoulf/epitech/cpp_spider/include/socket
//
// Made by Gandoulf
// Login   <gandoulf@epitech.net>
//
// Started on  Sat Nov  5 12:11:59 2016 Gandoulf
// Last update Fri Nov 11 14:48:04 2016 Gandoulf
//

#ifndef CLIENTTCPSOCKET_HPP_
# define CLIENTTCPSOCKET_HPP_

#include <list>
#include <thread>
#include <boost/asio.hpp>

#include "spider/packetSerializer.hpp"
#include "spider/packetUnserializer.hpp"

namespace spider
{
  namespace socket
  {
    class ClientTcpSocket
    {
    public:
      ClientTcpSocket(boost::asio::io_service& io_service,
	     boost::asio::ip::tcp::resolver::iterator endpoint);

      //methode to use the socket
      void connect(); // connect to the server
      void close(); // disconnet the client
      template<class packet>
      void write(spider::PacketSerializer<packet> data)
      {
	_messages.push_back(data.getPackedData());
	_messagesSize.push_back(data.getPacketSize());
	if (!_writing)
	  doWrite();
      }

      // methode to manage the service
      bool startedService() const;
      void startService();
      void closeService();

      //setter
      void setClientID(int id);
      void setEndPoint(boost::asio::ip::tcp::resolver::iterator const &endpoint);

    private:
      void identification();
      void readHeader();
      void readData();
      void doWrite();

    private:
      boost::asio::io_service &			_ioService;
      boost::asio::ip::tcp::socket		_socket;
      boost::asio::ip::tcp::resolver::iterator	_endpoint;
      int					_clientID;
      std::shared_ptr<std::thread>		_runningService;

      //packet
      std::list<spider::packedData>		_messages;
      std::list<int>				_messagesSize;
      bool					_writing;
      spider::PacketUnserializer		_packet;
      char					_data[128];
    };
  }
}

#endif /* CLIENTTCPSOCKET_HPP_ */
