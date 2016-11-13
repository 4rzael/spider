//
// SqlServer.hh for sqlServer in /home/debrab_t/rendu/TEK3/cpp_spider/bdd_connect
//
// Made by debrab_t
// Login   <debrab_t@epitech.net>
//
// Started on  Mon Nov  7 10:23:23 2016 debrab_t
// Last update Sat Nov 12 12:43:34 2016 Gandoulf
//

#ifndef SQLSERVER_HH_
# define SQLSERVER_HH_

#include "bdd_connect/SqlManager.hh"

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

#endif /* SQLSERVER_HH_ */
