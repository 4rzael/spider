#ifndef SPIDERSERVER_HPP_
# define SPIDERSERVER_HPP_

#include "socket/socketC/serverTcpSocketC.hpp"
#include <map>

namespace spider
{
	class SpiderServer
	{
	public:
		SpiderServer(int port);
		~SpiderServer();
		
		void start();
	private:
		spider::socket::ServerTcpSocket server;
		std::map<std::string, (*void)()> serverCMD;
	};
}

#endif /* SPIDERSERVER_HPP_ */