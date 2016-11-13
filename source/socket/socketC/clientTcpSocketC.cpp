//
// clientTcpSocket.cpp for  in /home/gandoulf/epitech/cpp_spider/source/socket
//
// Made by Gandoulf
// Login   <gandoulf@epitech.net>
//
// Started on  Wed Nov  9 15:32:04 2016 Gandoulf
// Last update Sun Nov 13 11:04:05 2016 Gandoulf
//

#include <cstdlib>
#include <iostream>

#include "socket/socketC/clientTcpSocketC.hpp"

namespace spider
{
  namespace socket
  {
    ClientTcpSocket::ClientTcpSocket(std::string adresse, int port)
      : _runningService(false), _adresse(adresse), _port(port)
    {
      memset(&_data[0], 0, 128);
      _client.OnReadPossible([this](Socket::Client &client, size_t length)
			     {
			       readHeader();
			     });
      _client.OnWritePossible([this](Socket::Client &client)
			      {
				doWrite();
			      });
      _client.OnStart([this](Socket::Client &client, std::string const &address, int port)
		      {
			connect();
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
      readData();
    }

    void ClientTcpSocket::readData()
    {
      memset(&_data[0], 0, 128);
      _client.read(static_cast<void *>(_data), _packet.getHeader().size -
		   sizeof(PackageHeader));
      _packet.setData(_data, _packet.getHeader().size - sizeof(PackageHeader));
      _packet.printPacketType();
    }

    void ClientTcpSocket::doWrite()
    {
      _Mqueue.lock();
      if (!_messages.empty())
	{
	  _client.write(_messages.front(), _messagesSize.front());
	  delete[] _messages.front();
	  _messages.pop_front();
	  _messagesSize.pop_front();
	}
      _Mqueue.unlock();
    }
  }
}
