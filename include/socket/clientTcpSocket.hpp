//
// clientTcpSocket.hpp for  in /home/gandoulf/epitech/cpp_spider/include/socket
//
// Made by Gandoulf
// Login   <gandoulf@epitech.net>
//
// Started on  Sat Nov  5 12:11:59 2016 Gandoulf
// Last update Mon Nov  7 15:15:00 2016 Gandoulf
//

#ifndef CLIENTTCPSOCKET_HPP_
# define CLIENTTCPSOCKET_HPP_

#include <thread>
#include <boost/asio.hpp>

#include "spider/packetSerializer.hpp"

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
	_ioService.post([this, data]()
			{
			  doWrite(data);
			});
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
      void read();
      template<class packet>
      void doWrite(spider::PacketSerializer<packet> data)
      {
	//spider::packedData packet;
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
				 });
      }

    private:
      boost::asio::io_service &			_ioService;
      boost::asio::ip::tcp::socket		_socket;
      boost::asio::ip::tcp::resolver::iterator	_endpoint;
      int					_clientID;
      std::shared_ptr<std::thread>		_runningService;
      //tmp
      char					msg[128];
      int					length;
    };
  }
}

#endif /* CLIENTTCPSOCKET_HPP_ */
