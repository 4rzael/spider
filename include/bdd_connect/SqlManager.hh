//
// SqlManager.hh for sqlManager in /home/debrab_t/rendu/TEK3/cpp_spider
//
// Made by debrab_t
// Login   <debrab_t@epitech.net>
//
// Started on  Sun Nov 13 14:37:03 2016 debrab_t
// Last update Sun Nov 13 14:38:29 2016 debrab_t
//

#ifndef SQLMANAGER_HH_
# define SQLMANAGER_HH_

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
  void	createTable(const std::string &, const std::string &) const;
  void	insertData(const std::string &tableName,
		   const std::string &rowsName,
		   const std::string &rowsRec) const;
  void	updateData(const std::string &tableName, const std::string &condition) const;
  void	removeData(const std::string &tableName, const std::string &condition) const;
  pqxx::result	selectData(const std::string &rowsName,
			   const std::string &fromAndCondition) const;
  void	closeManager() const;

private:
  pqxx::connection	*cInfo;
  pqxx::work		*wInfo;
};

#endif /* SQLMANAGER_HH_ */
