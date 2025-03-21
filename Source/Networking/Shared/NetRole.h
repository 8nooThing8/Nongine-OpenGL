#pragma once

#include <string>

#include "../Shared/Common.h"

typedef unsigned __int64 SOCKET;

#define DEFAULT_PORT 8080
#define DEFAULT_BUFLEN 512

#include <iostream>

#include <utility>

struct NetworkRecieveData
{
	char data[DEFAULT_BUFLEN];
	DataTypeSent dataType;
	int bytesSent;
	int objectID;

	NetworkRecieveData(int aDataType, int aBytesSent, int aObjectID) : dataType(static_cast<DataTypeSent>(aDataType)), bytesSent(aBytesSent), objectID(aObjectID)
	{

	}
};

class NetRole
{
public:
	NetRole() = default;
	~NetRole() = default;
	virtual int Init() = 0;

	virtual int SendData(const void* aData, int aBytesToSend, DataTypeSent aDataSent) = 0;

	virtual NetworkRecieveData RecieveData() = 0;

protected:
	SOCKET mySocket;
};