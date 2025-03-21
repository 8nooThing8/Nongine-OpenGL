// StartServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "Server\Server.h"
#include <WinSock2.h>

#include <thread>

int main()
{
	WSADATA wsadata;

	int iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);

	if (iResult != 0)
	{
		std::cout << "WSAStartup failed: " << iResult << std::endl;
		return false;
	}

	Server* server = new Server();

	std::cout << "Waiting for first client to conect..." << std::endl;

	iResult = server->Init();

	std::cout << "Started server succesfully" << std::endl;

	while (true)
	{
		server->RecieveData();
	}
}