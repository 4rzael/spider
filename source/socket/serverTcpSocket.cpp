//
// chat_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <utility>

#include "socket/serverTcpSocket.hpp"

using boost::asio::ip::tcp;

namespace spider
{
  namespace socket
  {
    //client user
    user::user(tcp::socket socket, std::set<user_ptr> & clients)
      : _socket(std::move(socket)), _clients(clients)
    {
    }

    void user::start()
    {
      _clients.insert(shared_from_this());
      readHeader();
    }

    void user::close()
    {
      _clients.erase(shared_from_this());
      _socket.close();
    }

    void user::readHeader()
    {
      auto self(shared_from_this());
      boost::asio::async_read(_socket,
			      boost::asio::buffer(_data, sizeof(PackageHeader)),
			      [this, self](boost::system::error_code ec, std::size_t length)
			      {
				if (!ec)
				  {
				    _packet.setHeader(_data, length);
				    PackageHeader header = _packet.getHeader();
				    std::cout << "MAGIC = " << header.magicNumber << std::endl;
				    std::cout << "size = " << header.size << std::endl;
				    std::cout << "id = " << header.id << std::endl;
				    readData();
				  }
			      });
    }

    void user::readData()
    {
      auto self(shared_from_this());
      boost::asio::async_read(_socket,
			      boost::asio::buffer(_data,_packet.getHeader().size -
						  sizeof(PackageHeader)),
			      [this, self](boost::system::error_code ec, std::size_t length)
			      {
				if (!ec)
				  {
				    std::cout << "header size" << length << std::endl;
				    _packet.setData(_data, length);
				    Test pkg = _packet.getData<Test>();
				    std::cout << "pkg = " << pkg.cmd[0] << pkg.cmd[1]
					      << pkg.cmd[2]  << std::endl;
				    std::cout << "msg = " << pkg.str << std::endl;
				    readHeader();
				  }
			      });
    }

    //server behavior

    ServerTcpSocket::ServerTcpSocket(boost::asio::io_service& io_service,
	   const tcp::endpoint& endpoint)
      : _acceptor(io_service, endpoint),
	_socket(io_service)
    {
      accept();
    }

    void ServerTcpSocket::accept()
    {
      _acceptor.async_accept(_socket,
			     [this](boost::system::error_code ec)
			     {
			       if (!ec)
				 {
				   std::make_shared<user>(std::move(_socket), _clients)->start();
				 }

			       accept();
			     });
    }
  }
}
