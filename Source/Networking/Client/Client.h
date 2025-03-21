#pragma once

#include "../Shared/NetRole.h"

typedef unsigned __int64 SOCKET;

class Client
{
public:
	Client() = default;
	~Client() = default;

	int Init();
	int SendData(const void* aData, int aBytesToSend, DataTypeSent aDataSent, int aGOID);

	NetworkRecieveData RecieveData();

private:
	SOCKET mySocket;
};

