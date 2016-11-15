//
// handleMsgToClient.hh for handleMSG in /home/debrab_t/rendu/TEK3/spider
//
// Made by debrab_t
// Login   <debrab_t@epitech.net>
//
// Started on  Mon Nov 14 22:45:44 2016 debrab_t
// Last update Tue Nov 15 09:45:54 2016 debrab_t
//

#ifndef HANDLEMSGTOCLIENT_HH_
# define HANDLEMSGTOCLIENT_HH_

# include "socket/socketC/serverTcpSocketC.hpp"
# include "utils/stringManager.hh"

# include <iostream>
# include <map>
# include <vector>
# include <string>
# include <sstream>

using spider::socket::ServerTcpSocket;

class	HandleCmdToClient
{
public:
  HandleCmdToClient();
  ~HandleCmdToClient();
  void	sendCmdToClient(std::string, ServerTcpSocket&);
  void	shut(ServerTcpSocket &server, std::vector<std::string> cmdSplit);
  void	talk(ServerTcpSocket &server, std::vector<std::string> cmdSplit);
  void	pong(ServerTcpSocket &server, std::vector<std::string> cmdSplit);
  void	sing(ServerTcpSocket &server, std::vector<std::string> cmdSplit);
  void	norm(ServerTcpSocket &server, std::vector<std::string> cmdSplit);
  void	mode(ServerTcpSocket &server, std::vector<std::string> cmdSplit);
  void	dec(ServerTcpSocket &server, std::vector<std::string> cmdSplit);
  void	feedCmdToClientMap();

private:
  typedef void	(HandleCmdToClient::*_cmdToClientFunc)(ServerTcpSocket &, std::vector<std::string>);
  std::map<std::string, _cmdToClientFunc> _cmdToClientMap;
  StringManager		_strMan;
};

#endif /* HANDLEMSGTOCLIENT_HH_ */
