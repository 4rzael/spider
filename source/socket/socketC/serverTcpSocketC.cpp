//
// serverTcpSocketC.cpp for  in /home/gandoulf/epitech/cpp_spider/source/socket/socketC
//
// Made by Gandoulf
// Login   <gandoulf@epitech.net>
//
// Started on  Sat Nov 12 11:24:36 2016 Gandoulf
// Last update Sun Nov 13 16:35:47 2016 debrab_t
//

#include "socket/socketC/serverTcpSocketC.hpp"

namespace spider
{
  namespace socket
  {
    user::user(Socket::Server & server, std::set<user_ptr> & clients, SqlServer &sqlServer,
	       int fd, std::mutex & Mclients)
      : _server(server), _clients(clients), _sqlServer(sqlServer), _fd(fd), _Mclients(Mclients)
    {
    }

    user_ptr user::start()
    {
      std::cout << "connection" << std ::endl;
      _Mclients.lock();
      _clients.insert(shared_from_this());
      _Mclients.unlock();
      return (shared_from_this());
    }

    void user::close()
    {
      if (_fd != 0)
	{
	  std::cout << "disconnection" << std ::endl;
	  _server.disconnect(_fd);
	  _Mclients.lock();
	  _clients.erase(shared_from_this());
	  _Mclients.unlock();
	  _fd = 0;
	}
    }

    void user::read()
    {
      readHeader();
    }

    void user::doWrite()
    {
      _Mqueue.lock();
      if (!_messages.empty())
	{
	  _server.write(_fd, _messages.front(), _messagesSize.front());
	  delete[] _messages.front();
	  _messages.pop_front();
	  _messagesSize.pop_front();
	}
      _Mqueue.unlock();
    }

    //private

    void user::readHeader()
    {
      memset(&_data[0], 0, 128);
      _server.read(_fd, static_cast<void *>(_data), sizeof(PackageHeader));
      _packet.setHeader(_data, sizeof(PackageHeader));
      readData();
    }

    void user::readData()
    {
      memset(&_data[0], 0, 128);
      _server.read(_fd, static_cast<void *>(_data), _packet.getHeader().size -
		   sizeof(PackageHeader));
      _packet.setData(_data, _packet.getHeader().size - sizeof(PackageHeader));
      _packet.printPacketType();
      if (!_sqlServer.handleData(_packet, shared_from_this()))
	close();
    }

    //server behavior

    ServerTcpSocket::ServerTcpSocket(int port, size_t maxClient)
      : _port(port), _maxClient(maxClient)
    {
      _runningService = false;
      _server.OnConnect([this](Socket::Server & server, int fd)
		       {
			 user_ptr newClients;
			 newClients = accept(fd);
			 _clientsFD.insert(std::pair<int,user_ptr>(fd, newClients));
		       });
      _server.OnDisconnect([this](Socket::Server & server, int fd)
			  {
			    _clientsFD[fd].get()->close();
			    auto deconnectedClient = _clientsFD.find(fd);
			    _clientsFD.erase(deconnectedClient);
			  });
      _server.OnReadPossible([this](Socket::Server & server, int fd, size_t length)
			    {
			      _clientsFD[fd].get()->read();
			    });
      _server.OnWritePossible([this](Socket::Server & server, int fd)
			     {
			       _clientsFD[fd].get()->doWrite();
			     });
      _server.OnStart([this](Socket::Server & server, int port)
		     {
		       std::cout << "starting server on port " << port << std::endl;
		     });
    }

    void ServerTcpSocket::close()
    {
      if (_runningService)
	{
	  std::cout << "closing" << std::endl;
	  _runningService = false;
	  _server.stop();
	  _Mclients.lock();
	  _clients.clear();
	  _Mclients.unlock();
	  _clientsFD.clear();
	}
      else
	std::cout << "server already stop" << std::endl;
    }

    void ServerTcpSocket::startService()
    {
      if (!_runningService)
	{
	  _runningService = true;
	  std::cout << "start thread and service" << std::endl;
	  _server.start(_port, _maxClient);
	}
      else
	std::cout << "service already started" << std ::endl;
    }

    void ServerTcpSocket::closeService()
    {
      close();
    }

    user_ptr ServerTcpSocket::accept(int fd)
    {
      return (std::make_shared<user>(_server, _clients, _sqlServer, fd, _Mclients)->start());
    }
  }
}
