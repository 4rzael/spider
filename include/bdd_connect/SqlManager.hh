//
// SqlLink.hh for SqlLink in /home/debrab_t/rendu/TEK3/spider/bdd_connect
//
// Made by debrab_t
// Login   <debrab_t@epitech.net>
//
// Started on  Wed Oct 26 16:26:01 2016 debrab_t
// Last update Sat Nov 12 12:43:02 2016 Gandoulf
//
#ifndef SQLMANAGER_HH_
# define SQLMANAGER_HH_

#include <string>
#include <iostream>
#include <pqxx/pqxx>
#include <map>
#include <vector>

class SqlManager
{
public:
  SqlManager(const std::string &);
  SqlManager();
  ~SqlManager();
  bool	connect(const std::string &connect);
  void	createTable(const std::string &, const std::string &);
  void	insertData(const std::string &tableName,
		   const std::string &rowsName,
		   const std::string &rowsRec);
  void	updateData(const std::string &tableName, const std::string &condition);
  void	removeData(const std::string &tableName, const std::string &condition);
  pqxx::result	selectData(const std::string &rowsName,
			   const std::string &fromAndCondition);
  void	closeManager();

private:
  pqxx::connection	*cInfo;
  pqxx::work		*wInfo;
};

#endif /* SQLMANAGER_HH_ */
