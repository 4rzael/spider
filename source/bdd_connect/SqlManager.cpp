//
// SqlLink.cpp for SqlLink in /home/debrab_t/rendu/TEK3/spider/bdd_connect
//
// Made by debrab_t
// Login   <debrab_t@epitech.net>
//
// Started on  Wed Oct 26 16:26:19 2016 debrab_t
// Last update Sun Nov 13 00:04:23 2016 debrab_t
//

#include "bdd_connect/SqlManager.hh"
#include <string>
#include <iostream>

SqlManager::SqlManager(const std::string &connect)
{
  try
    {
      cInfo = new pqxx::connection(connect);
      if (cInfo->is_open())
	std::cout << "Opened database successfully: " << cInfo->dbname() << std::endl;
    }
  catch (const std::exception &e)
    {
      std::cerr << e.what() << std::endl;
    }
}

SqlManager::SqlManager()
{
  cInfo = NULL;
  wInfo = NULL;
}

SqlManager::~SqlManager()
{
  try
    {
      if (cInfo)
	cInfo->disconnect();
    }
  catch (const std::exception &e)
    {
      std::cerr << e.what() << std::endl;
    }
}

bool		SqlManager::connect(const std::string &connect)
{
  try
    {
      cInfo = new pqxx::connection(connect);
      if (cInfo->is_open())
	std::cout << "Opened database successfully: " << cInfo->dbname() << std::endl;
      return (true);
    }
  catch (const std::exception &e)
    {
      std::cerr << e.what() << std::endl;
      return (false);
    }
}

void		SqlManager::closeManager() const
{
  try
    {
      if (cInfo)
	cInfo->disconnect();
    }
  catch (const std::exception &e)
    {
      std::cerr << e.what() << std::endl;
    }
}

void		SqlManager::createTable(const std::string &tableName, const std::string &rows) const
{
  std::string	sql;
  pqxx::work	l_work(*cInfo);

  sql = "CREATE TABLE " + tableName + "(" + rows + ")";
  try
    {
      l_work.exec(sql);
      l_work.commit();
      std::cout << "Table created successfully" << std::endl;
    }
  catch (const std::exception &e)
    {
      l_work.abort();
      std::cerr << e.what() << std::endl;
    }
}

void		SqlManager::insertData(const std::string &tableName,
				       const std::string &rowsName,
				       const std::string &rowsRec) const
{
  std::string	sql;
  pqxx::work	l_work(*cInfo);

  sql = "INSERT INTO " + tableName +"(" + rowsName + ") VALUES (" + rowsRec + ");";
  try
    {
      l_work.exec(sql);
      l_work.commit();
      std::cout << "Record created successfully" << std::endl;
    }
  catch (const std::exception &e)
    {
      l_work.abort();
      std::cerr << e.what() << std::endl;
    }
}

void		SqlManager::updateData(const std::string &tableName,
				       const std::string &condition) const
{
  std::string	sql;
  pqxx::work	l_work(*cInfo);

  sql = "UPDATE " + tableName + " set " + condition;
  try
    {
      l_work.exec( sql );
      l_work.commit();
      std::cout << "Records updated successfully" << std::endl;
    }
  catch (const std::exception &e)
    {
      l_work.abort();
      std::cerr << e.what() << std::endl;
    }
}

void		SqlManager::removeData(const std::string &tableName,
				       const std::string &condition) const
{
  std::string	sql;
  pqxx::work	l_work(*cInfo);

  sql = "DELETE from " + tableName +" WHERE " + condition;
  try
    {
      l_work.exec( sql );
      l_work.commit();
      std::cout << "Records deleted successfully" << std::endl;
    }
  catch (const std::exception &e)
    {
      l_work.abort();
      std::cerr << e.what() << std::endl;
    }
}

pqxx::result		SqlManager::selectData(const std::string &rowsName,
					       const std::string &fromAndCondition) const
{
  std::string		sql;
  pqxx::nontransaction	*nonT;
  pqxx::result		*ret;

  sql = "SELECT " + rowsName + " FROM " + fromAndCondition;
  try
    {
      nonT = new pqxx::nontransaction(*cInfo);
      ret = new pqxx::result(nonT->exec(sql));
      delete (nonT);
     }
  catch (const std::exception &e)
    {
      std::cerr << e.what() << std::endl;
    }
  return (*ret);
}
