//
// SqlServer.hh for sqlServer in /home/debrab_t/rendu/TEK3/cpp_spider/bdd_connect
//
// Made by debrab_t
// Login   <debrab_t@epitech.net>
//
// Started on  Mon Nov  7 10:23:23 2016 debrab_t
// Last update Sun Nov 13 19:28:16 2016 debrab_t
//

#ifndef SQLSERVER_HH_
# define SQLSERVER_HH_

# include "bdd_connect/SqlManager.hh"
# include "utils/stringManager.hh"
# include "bdd_connect/HandleFileServer.hh"

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
  bool	connectClient(spider::PacketUnserializer &);
  bool	addMouseMouvement(spider::PacketUnserializer &);
  bool	addMouseClick(spider::PacketUnserializer &);
  bool	addKeyboardString(spider::PacketUnserializer &);
  bool	disconnectClient(spider::PacketUnserializer &);
  bool	response(spider::PacketUnserializer &);
  bool	handleData(spider::PacketUnserializer &,
		   std::shared_ptr<spider::socket::user> );
  void	feedPointMap();
  bool	isClient(const std::string &);
  bool	isClientState(const std::string &);
  std::string		insertStringAfterChar(std::string, char, std::string);

private:
  SqlManager		sqlMan;
  StringManager		strMan;
  HandleFileServer	dataToFile;
  typedef bool	(SqlServer::*bddFunc)(spider::PacketUnserializer &);
  std::map<std::string, bddFunc> _pointMap;
  std::shared_ptr<spider::socket::user> _user;
  bool			_cnt;
  unsigned int		_tmsTOC;
  std::string		_keyToc;
};

#endif /* SQLSERVER_HH_ */
