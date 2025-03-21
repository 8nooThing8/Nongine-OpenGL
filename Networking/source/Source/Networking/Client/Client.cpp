#include "Client.h"

#include <iostream>

#include "Vector\Vector2.hpp"
#include "Vector\Vector3.hpp"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

struct sockaddr_in clientAddr;
struct sockaddr_in serverAddr;

int Client::Init()
{
	mySocket = INVALID_SOCKET;

	//struct addrinfo* result = NULL, * ptr = NULL, hints;

	//ZeroMemory(&hints, sizeof(hints));
	//hints.ai_family = AF_INET;
	//hints.ai_socktype = SOCK_DGRAM;
	//hints.ai_protocol = IPPROTO_UDP;

	char hostname[256];
	if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) 
	{
		std::cout << "Failed getting current" << std::endl;
	}

	struct addrinfo hints = {}, * result = nullptr;
	hints.ai_family = AF_INET; // Use AF_INET for IPv4, or AF_INET6 for IPv6

	getaddrinfo(hostname, nullptr, &hints, &result);


	// Auto ip checker to get the first ip adress in your ip adress in console
	char ip[INET_ADDRSTRLEN];
	for (struct addrinfo* ptr = result; ptr != nullptr; ptr = ptr->ai_next) 
	{
		if (ptr->ai_family == AF_INET) 
		{ 
			struct sockaddr_in* sockaddr_ipv4 = reinterpret_cast<struct sockaddr_in*>(ptr->ai_addr);
			inet_ntop(AF_INET, &(sockaddr_ipv4->sin_addr), ip, INET_ADDRSTRLEN);
			std::cout << "IP Address: " << ip << std::endl;
			break;
		}
	}

	ip;

	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(0);
	inet_pton(AF_INET, "127.0.0.1", &clientAddr.sin_addr.s_addr);
	
	//std::cout << "ip: ";
	std::string ipString = "127.0.0.1";

	//std::cout << "\nport: ";
	//std::string portString = ;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(DEFAULT_PORT);
	inet_pton(AF_INET, ipString.c_str(), &serverAddr.sin_addr.s_addr);

	mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mySocket == INVALID_SOCKET)
	{
		printf("Socket creation failed: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	if (bind(mySocket, (sockaddr*)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR)
	{
		std::cout << "Client bind failed: " << WSAGetLastError() << std::endl;
		return 1;
	}
	else
	{
		socklen_t len = sizeof(clientAddr);
		getsockname(mySocket, (sockaddr*)&clientAddr, &len);
		std::cout << "Client bound to port: " << ntohs(clientAddr.sin_port) << std::endl;
	}

	//iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	//if (iResult != 0)
	//{
	//	std::cout << "getaddrinfo failed with error code: " << iResult << std::endl;
	//	WSACleanup();
	//	return 404;
	//}

	//ptr = result;

	//mySocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	//if (mySocket == INVALID_SOCKET)
	//{
	//	std::cout << "error at socket(): " << WSAGetLastError() << std::endl;
	//	freeaddrinfo(result);
	//	WSACleanup();
	//	return 1;
	//}

	//iResult = connect(mySocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	//if (iResult == SOCKET_ERROR)
	//{
	//	closesocket(mySocket);
	//	mySocket = INVALID_SOCKET;
	//}

	//freeaddrinfo(result);

	//if (mySocket == INVALID_SOCKET)
	//{
	//	//std::cout << "Unable to connect to the server!" << std::endl;
	//	//WSACleanup();
	//	return 1;
	//}

	return 0;
}

int Client::SendData(const void* aData, int aBytesToSend, DataTypeSent aDataSent, int aGOID)
{
	char valueToCopyInto[DEFAULT_BUFLEN];

	const char* charData;

	int type = static_cast<int>(aDataSent);

	charData = reinterpret_cast<const char*>(aData);
	memcpy(valueToCopyInto + 4, charData, aBytesToSend);

	memcpy(valueToCopyInto, &type, sizeof(int));

	int iResult = sendto(mySocket, valueToCopyInto, aBytesToSend + 4, 0, (SOCKADDR*)&serverAddr, (int)sizeof(serverAddr));

	//if (iResult == SOCKET_ERROR)
	//{
	//	std::cout << "Could not send data to server" << std::endl;
	//	closesocket(mySocket);
	//	WSACleanup();
	//	return 1;
	//}

	//std::cout << "CLIENT: " << aData << std::endl;

	return 0;
}

NetworkRecieveData Client::RecieveData()
{
	int iResult;

	int recvbufllen = DEFAULT_BUFLEN;
	char recbuf[DEFAULT_BUFLEN];

	sockaddr_in senderAddr;
	int senderAddrSize = sizeof(senderAddr);

	iResult = recvfrom(mySocket, recbuf, recvbufllen, 0, (sockaddr*)&senderAddr, &senderAddrSize);

	//if (iResult == SOCKET_ERROR)
	//{
	//	std::cout << WSAGetLastError() << std::endl;
	//}

	if (iResult > 0)
	{
		char dataType[sizeof(int)];
		std::memcpy(&dataType, recbuf, 4);

		//char GOID[sizeof(int)];
		//std::memcpy(&GOID, recbuf + 4, 4);

		const int* dataTypeintPtr = reinterpret_cast<const int*>(dataType);
		int dataTypeInt = *dataTypeintPtr;

		//const int* objIDintPtr = reinterpret_cast<const int*>(GOID);
		//int objIDInt = *objIDintPtr;

		NetworkRecieveData data{ dataTypeInt, iResult, 0 };

		std::memcpy(data.data, recbuf + sizeof(int), iResult);

		if (iResult > 0)
		{
			return data;
		}
		else if (iResult == 0)
			std::cout << "Connection closed" << std::endl;
		else
			std::cout << "recv failed: " << WSAGetLastError() << std::endl;
	}
	

	return { 0, 0, 0 };
}
