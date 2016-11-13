//
// HandleFileServer.hh for handlefile in /home/debrab_t/rendu/TEK3/cpp_spider
//
// Made by debrab_t
// Login   <debrab_t@epitech.net>
//
// Started on  Fri Nov 11 15:02:21 2016 debrab_t
// Last update Sun Nov 13 19:27:22 2016 debrab_t
//

#ifndef HANDLEFILESERVER_HH_
# define HANDLEFILESERVER_HH_

# include "bdd_connect/SharedData.hh"
# include "utils/stringManager.hh"

# include <iostream>
# include <vector>
# include <map>
# include <fstream>

class	HandleFileServer
{
public:
  HandleFileServer();
  ~HandleFileServer();
  void	insertDataClient(int, const std::map<HandleData::column, std::string> &);
  void	verifFirstLine(int, std::ifstream &);

private:
  std::map<HandleData::column, std::string>		_model;
  StringManager						strMan;
};

#endif /* HANDLEFILESERVER_HH_ */
