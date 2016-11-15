//
// handleMsgToClient.cpp for msg in /home/debrab_t/rendu/TEK3/spider
//
// Made by debrab_t
// Login   <debrab_t@epitech.net>
//
// Started on  Mon Nov 14 23:04:37 2016 debrab_t
// Last update Tue Nov 15 10:44:15 2016 debrab_t
//

#include "spider/handleCmdToClient.hh"

HandleCmdToClient::HandleCmdToClient()
{
  feedCmdToClientMap();
}

HandleCmdToClient::~HandleCmdToClient()
{
}

void	HandleCmdToClient::shut(ServerTcpSocket &server, std::vector<std::string> cmdSplit)
{
  if (cmdSplit.size() == 2 && _strMan.checkNumber(cmdSplit[1]))
    {
      PackageCMDTime	packTime;
      PackageHeader	hea;

      std::strncpy(packTime.cmd, "SHU", 3);
      packTime.timer = std::stoi(cmdSplit[1]);
      spider::PacketSerializer<PackageCMDTime> packet(sizeof(PackageHeader) + sizeof(PackageCMDTime), 1999888256, packTime);
      server.write<PackageCMDTime>(packet);
      std::cout << "[SEND] SHUT" << std::endl;
    }
}

void	HandleCmdToClient::talk(ServerTcpSocket &server, std::vector<std::string> cmdSplit)
{
  if (cmdSplit.size() == 1)
    {
      PackageCMD	packCMD;
      PackageHeader	hea;

      std::strncpy(packCMD.cmd, "TAL", 3);
      spider::PacketSerializer<PackageCMD> packet(sizeof(PackageHeader) + sizeof(PackageCMD), 1999888256, packCMD);
      server.write<PackageCMD>(packet);
      std::cout << "[SEND] TALK" << std::endl;
    }
}


void	HandleCmdToClient::pong(ServerTcpSocket &server, std::vector<std::string> cmdSplit)
{
  if (cmdSplit.size() == 2 && cmdSplit[1].length() < 20)
    {
      PackageCMDPING	packPing;
      PackageHeader	hea;

      std::strncpy(packPing.cmd, "PIN", 3);
      std::strncpy(packPing.id, cmdSplit[1].c_str(), cmdSplit[1].length());
      spider::PacketSerializer<PackageCMDPING> packet(sizeof(PackageHeader) + sizeof(PackageCMDPING), 1999888256, packPing);
      server.write<PackageCMDPING>(packet);
      std::cout << "[SEND] PONG" << std::endl;
    }
}

void	HandleCmdToClient::sing(ServerTcpSocket &server, std::vector<std::string> cmdSplit)
{
  if (cmdSplit.size() == 2 && _strMan.checkNumber(cmdSplit[1]))
    {
      PackageCMDTime	packTime;
      PackageHeader	hea;

      std::strncpy(packTime.cmd, "SIN", 3);
      packTime.timer = std::stoi(cmdSplit[1]);
      spider::PacketSerializer<PackageCMDTime> packet(sizeof(PackageHeader) + sizeof(PackageCMDTime), 1999888256, packTime);
      server.write<PackageCMDTime>(packet);
      std::cout << "[SEND] SING" << std::endl;
    }
}

void	HandleCmdToClient::norm(ServerTcpSocket &server, std::vector<std::string> cmdSplit)
{
}

void	HandleCmdToClient::mode(ServerTcpSocket &server, std::vector<std::string> cmdSplit)
{
}

void	HandleCmdToClient::dec(ServerTcpSocket &server, std::vector<std::string> cmdSplit)
{
  if (cmdSplit.size() == 1)
    {
      PackageCMDLogOut	packDec;
      PackageHeader	hea;

      std::strncpy(packDec.cmd, "DEC", 3);
      spider::PacketSerializer<PackageCMDLogOut> packet(sizeof(PackageHeader) + sizeof(PackageCMDLogOut), 1999888256, packDec);
      server.write<PackageCMDLogOut>(packet);
      std::cout << "[SEND] DEC" << std::endl;
    }
}

void	HandleCmdToClient::feedCmdToClientMap()
{
  _cmdToClientMap.insert(std::pair<std::string, _cmdToClientFunc>("SHU", &HandleCmdToClient::shut));
  _cmdToClientMap.insert(std::pair<std::string, _cmdToClientFunc>("TAL", &HandleCmdToClient::talk));
  _cmdToClientMap.insert(std::pair<std::string, _cmdToClientFunc>("PIN", &HandleCmdToClient::pong));
  _cmdToClientMap.insert(std::pair<std::string, _cmdToClientFunc>("SIN", &HandleCmdToClient::sing));
  _cmdToClientMap.insert(std::pair<std::string, _cmdToClientFunc>("NOR", &HandleCmdToClient::norm));
  _cmdToClientMap.insert(std::pair<std::string, _cmdToClientFunc>("MOD", &HandleCmdToClient::mode));
  _cmdToClientMap.insert(std::pair<std::string, _cmdToClientFunc>("DEC", &HandleCmdToClient::dec));
}

void	HandleCmdToClient::sendCmdToClient(std::string cmd, ServerTcpSocket &server)
{
  std::stringstream ss(cmd);
  std::string tok;
  std::vector<std::string> cmdSplit;

  while (std::getline(ss, tok, ' '))
    {
      cmdSplit.push_back(tok);
    }
  if (cmdSplit.size() > 0
      && _cmdToClientMap.find(cmdSplit[0]) != _cmdToClientMap.end())
    ((*this).*_cmdToClientMap.find(cmdSplit[0])->second)(server, cmdSplit);
}
