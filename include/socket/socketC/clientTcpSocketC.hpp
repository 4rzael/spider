//
// clientTcpSocket.hpp for  in /home/gandoulf/epitech/cpp_spider/include/socket
//
// Made by Gandoulf
// Login   <gandoulf@epitech.net>
//
// Started on  Sat Nov  5 12:11:59 2016 Gandoulf
// Last update Sun Nov 13 15:02:35 2016 Gandoulf
//

#ifndef CLIENTTCPSOCKET_HPP_
# define CLIENTTCPSOCKET_HPP_

#include "spider/packetUnserializer.hpp"
#include "spider/packetSerializer.hpp"
#include "Network/sockets/Client.hpp"

#include <list>
#include <mutex>

namespace spider
{
  namespace socket
  {
    class ClientTcpSocket
    {
    public:
      ClientTcpSocket(std::string adresse, int port);

      //methode to use the socket
      void connect(); // connect to the server
      void close(); // disconnet the client
      template<class packet>
      void write(spider::PacketSerializer<packet> data)
      {
	_Mqueue.lock();
	_messages.push_back(data.getPackedData(1));
	_messagesSize.push_back(data.getPacketSize());
	_Mqueue.unlock();
      }

      // methode to manage the service
      bool startedService() const;
      void startService();
      void closeService();

      //setter
      void setClientID(int id);
      void setServer(std::string adresse, int port);

    private:
      void identification();
      void readHeader();
      void readData();
      void doWrite();

    private:
      Socket::Client				_client;
      std::string				_adresse;
      int					_port;
      int					_clientID;
      bool					_runningService;

      //packet
      std::list<char *>				_messages;
      std::list<int>				_messagesSize;
      spider::PacketUnserializer		_packet;
      char					_data[128];
      std::mutex				_Mqueue;
    };
  }
}

#endif /* CLIENTTCPSOCKET_HPP_ */
