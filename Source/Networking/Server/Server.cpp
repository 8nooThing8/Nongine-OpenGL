#include "Server.h"

#include <thread>

#include <iostream>

#include "Vector\Vector2.hpp"
#include "Vector\Vector3.hpp"

#include <sstream>

#include <ws2tcpip.h>
#include <iphlpapi.h>

sockaddr_in local;

int Server::Init()
{
	mySocket = INVALID_SOCKET;

	int iResult;

	local.sin_family = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port = htons(DEFAULT_PORT);

	/*iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		std::cout << "WSAStartup failed: " << iResult << std::endl;
		WSACleanup();
		return 1;
	}*/

	mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (mySocket == INVALID_SOCKET)
	{
		std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
		//freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	iResult = bind(mySocket, (sockaddr*)&local, sizeof(local));
	if (iResult != 0)
	{
		std::cout << "Error binding a socket error code: " << WSAGetLastError() << std::endl;
		//freeaddrinfo(result);
		closesocket(mySocket);
		WSACleanup();
		return 1;
	}



	//freeaddrinfo(result);

	/*if (listen(mySocket, SOMAXCONN) == SOCKET_ERROR)
	{
		std::cout << "Listen failed with error: " << WSAGetLastError() << std::endl;
		closesocket(mySocket);
		WSACleanup();
		return 1;
	}

	clientSockets.push_back(SOCKET());
	clientSockets.back() = accept(mySocket, NULL, NULL);

	if (clientSockets.back() == INVALID_SOCKET)
	{
		std::cout << "accept failed, error: " << WSAGetLastError() << std::endl;
		closesocket(mySocket);
		WSACleanup();
		return 1;
	}*/

	clientsConnected++;

	//__debugbreak();

	//auto connectClientThread = std::thread(&Server::ConnectNewClients, this);
	//connectClientThread.detach();

	return 0;
}

int Server::SendData(const void* aData, int aBytesToSend, DataTypeSent /*aDataSent*/, int* portsToIgnore, int /*aCountPortToIgnore*/)
{
	int iResult = 0;

	char valueToCopyInto[DEFAULT_BUFLEN] = { 0 };

	memcpy(valueToCopyInto, aData, aBytesToSend);

	for (auto& client : myPortToSockAddr)
	{
		if (client.first == *portsToIgnore)
		{
			continue;
		}

		auto& clinetAddr = *client.second;

		iResult = sendto(mySocket, valueToCopyInto, aBytesToSend, 0, (sockaddr*)&clinetAddr, sizeof(clinetAddr));
	}

	if (iResult == SOCKET_ERROR)
	{
		std::cout << "Could not send data to client" << std::endl;
		closesocket(mySocket);

		/*for (size_t i = 0; i < clientSockets.size(); i++)
		{
			closesocket(clientSockets[i]);
		}*/
		WSACleanup();
		return 1;
	}


	return 0;
}

NetworkRecieveData Server::RecieveData()
{
	int iResult = 0;

	for (int i = 0; i < clientsConnected; i++)
	{
		//FD_SET(clientSockets[i], &readSet);
	}

	int recvbufllen = DEFAULT_BUFLEN;
	char recbuf[DEFAULT_BUFLEN];

	sockaddr_in clientAddr;
	int clientAddrSize = sizeof(clientAddr);

	iResult = recvfrom(mySocket, recbuf, sizeof(recbuf), 0, (sockaddr*)&clientAddr, &clientAddrSize);

	int port;

//	myClients.emplace(port);

	port = ntohs(clientAddr.sin_port);

	if (!myClients.contains(port))
	{
		std::cout << "new client" << std::endl;

		inet_ntop(AF_INET, &clientAddr.sin_addr, myClients[port], INET_ADDRSTRLEN);
		myPortToSockAddr.emplace(port, std::make_shared<sockaddr_in>(clientAddr));
	}

	if (iResult > 0)
	{
		int dataAmountRecieved = iResult;

		//auto stringMessage = std::string(recbuf).substr(0, iResult);
		//auto message = stringMessage.c_str();

		iResult = SendData(recbuf, dataAmountRecieved, static_cast<DataTypeSent>(0), &port, 1);

		return { 0, dataAmountRecieved, 0 };
	}

	return { 0, 0, 0 };
}

void Server::ConnectNewClients()
{
	while (true)
	{
		/*	clientSockets.push_back(SOCKET());
			clientSockets.back() = accept(mySocket, NULL, NULL);

			clientsConnected++;*/

			/*if (clientSockets.back() == INVALID_SOCKET)
			{
				std::cout << "accept faile error: " << WSAGetLastError() << std::endl;
				closesocket(mySocket);
				WSACleanup();
				return;
			}*/
	}
}
