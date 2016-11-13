#pragma once

#ifndef CLIENT_SPIDER_HPP_
# define CLIENT_SPIDER_HPP_

# include <lmcons.h>
# include <queue>

# include "client/WindowsInputHandler.hpp"

namespace spider
{
	class ClientSpider
	{
	public:
		ClientSpider(LPVOID param);
		~ClientSpider();

		bool testConnection();
		bool protocolConnection();
		char *getMessageFromServer();
		int run();

	private:
		WindowsInputHandler *inputHandler;
		TCHAR userName[UNLEN + 1];
		DWORD userNameLength = UNLEN + 1;
		std::mutex *_mtx;
		std::queue<char *> &_rdQ;
	};
}

#endif // !CLIENT_SPIDER_HPP_