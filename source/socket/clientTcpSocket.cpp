#include <cstdlib>
#include <iostream>

#include "socket/clientTcpSocket.hpp"


using boost::asio::ip::tcp;

namespace spider
{
  namespace socket
  {
    ClientTcpSocket::ClientTcpSocket(boost::asio::io_service& io_service,
	   tcp::resolver::iterator endpoint)
      : _ioService(io_service), _endpoint(endpoint), _socket(io_service)
    {
      memset(&msg[0], 0, 128);
    }

    void ClientTcpSocket::close()
    {
      _ioService.post([this]() { _socket.close(); });
    }

    void ClientTcpSocket::connect()
    {
      boost::asio::async_connect(_socket, _endpoint,
				 [this](boost::system::error_code error, tcp::resolver::iterator)
				 {
				   if (!error) {
				     std::cout << "connection" << std::endl;
				     read();
				   }
				   else
				     std::cout << "error in co" << std::endl;
				 });
    }

    /*void ClientTcpSocket::write(PacketSerializer const &data)
    {
      _ioService.post([this, data]()
		      {
			std::cout << "writing in socket" << msg << std::endl;
			doWrite(data);
		      });
		      }*/

    bool ClientTcpSocket::startedService() const
    {
      return !!_runningService;
    }

    void ClientTcpSocket::startService()
    {
      if (!_runningService)
	{
	  _runningService = std::shared_ptr<std::thread>
	    (new std::thread([this]()
	    {
	      std::cout << "start thread and service" << std::endl;
	      _ioService.run();
	      std::cout << "stop running" << std::endl;
	    }));
	}
      else
	std::cout << "service already started" << std ::endl;
    }

    void ClientTcpSocket::closeService()
    {
      if (_runningService)
	{
	  _runningService->join();
	  _runningService.reset();
	}
      else
	std::cout << "nothing to release" << std::endl;
    }

    void ClientTcpSocket::setClientID(int id)
    {
      _clientID = id;
    }

    void ClientTcpSocket::setEndPoint(boost::asio::ip::tcp::resolver::iterator const &endpoint)
    {
      _endpoint = endpoint;
    }

    void ClientTcpSocket::read()
    {
      boost::asio::async_read(_socket, boost::asio::buffer(msg, 12),
			      [this](boost::system::error_code error, std::size_t _length)
			      {
				std::cout << "length = " << _length << std::endl;
				//std::cout << msg << std::endl;
				if (!error)
				  {
				    std::cout.write(msg, 5);
				    std::cout << std::endl;
				    read();
				  }
				else {
				  close();
				}
			      });
    }

    /*void doWrite(PacketSerializer const &data)
    {
      std::cout << "trying to write" << std::endl;
      }*/
  }
}
