#pragma once

#include <set>

#include <string>

#include "../Shared/NetRole.h"

#include <memory>
#include <unordered_map>

#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 22
#endif

typedef unsigned __int64 SOCKET;

struct sockaddr_in;

//struct ClientInfo
//{
//	ClientInfo() = default;
//	ClientInfo(ClientInfo&) = default;
//
//	ClientInfo(const char* aIp, int aPort) : port(aPort)
//	{
//		std::memcpy(IP, aIp, INET_ADDRSTRLEN);
//	}
//
//	char IP[INET_ADDRSTRLEN];
//	int port;
//};

//inline bool operator <(ClientInfo aclient0, ClientInfo aclient1)
//{
//	if (aclient0.port < aclient1.port)
//		return true;
//
//	return false;
//}

class Server
{
public:
	Server() = default;
	~Server() = default;

	int Init();
	int SendData(const void* aData, int aBytesToSend, DataTypeSent aDataSent, int* portsToIgnore, int aCountPortToIgnore);

	NetworkRecieveData RecieveData();

	void ConnectNewClients();

private:
	std::unordered_map<int, char[INET_ADDRSTRLEN]> myClients;
	std::unordered_map<int, std::shared_ptr<sockaddr_in>> myPortToSockAddr;

	int clientsConnected;
	SOCKET mySocket;
};