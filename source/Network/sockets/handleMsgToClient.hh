//
// handleMsgToClient.hh for handleMSG in /home/debrab_t/rendu/TEK3/spider
//
// Made by debrab_t
// Login   <debrab_t@epitech.net>
//
// Started on  Mon Nov 14 22:45:44 2016 debrab_t
// Last update Mon Nov 14 23:57:10 2016 debrab_t
//

#ifndef HANDLEMSGTOCLIENT_HH_
# define HANDLEMSGTOCLIENT_HH_

class	HandleMsgToClient
{
public:
  HandleMsgToClient();
  ~HandleMsgToClient();
  void	sendCmdToClient(std::strin &);

private:
  void	shut();
  void	talk();
  void	pong();
  void	sing();
  void	norm();
  void	mode();
  void	dec();
  void	feedMsgToClientMap();
  typedef void	(HandleMsgToClient::*_msgToClientFunc)();
  std::map<std::string, _msgToClientFunc> _msgToClientMap;
};

#endif /* HANDLEMSGTOCLIENT_HH_ */
