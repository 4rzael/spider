#include "spiderServer.hpp"

namespace spider
{
	SpiderServer::SpiderServer(int port)
		: _server(port)
	{

	}

	SpiderServer::~SpiderServer()
	{

	}

	void SpiderServer::start()
	{
		_server.startService();
		char line[100];
		while (std::cin.getline(line, 100))
		{
			std::cout << "coucou" << std::endl;
		}
	}
}