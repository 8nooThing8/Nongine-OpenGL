#pragma once

#ifdef USENET
#include <string>

#include "../Shared/Common.h"

#define DEFAULT_PORT 8080
#define DEFAULT_BUFLEN 512

#include <iostream>

#include <utility>

#ifndef BOOST_BIND_GLOBAL_PLACEHOLDERS
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#endif

#ifndef BOOST_NO_EXCEPTION
#define BOOST_NO_EXCEPTION
#endif

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>

#include <optional>
#include <vector>

namespace boostAsio = boost::asio;
namespace boostIp = boostAsio::ip;

#include <Timer.h>

struct NetroleStorage
{
	NetroleStorage(int aSize, unsigned aPort, unsigned aNetworkID) : dataSize(aSize), port(aPort), networkID(aNetworkID), timeAtSend(CommonUtilities::Timer::GetTotalTime()), timeForPing(CommonUtilities::Timer::GetTotalTime())
	{
	}

	static const short timesUntilTimeout = 1;

	char dataSent[DEFAULT_BUFLEN];
	int dataSize;
	DataTypeSent typeSent;
	unsigned networkID;
	unsigned port;
	float timeAtSend = 0;
	float timeForPing = 0;
	short currentTimeoutCount = 0;
};

struct NetworkData
{
	// To be able to sent multiple data at once
	struct PerObjectData
	{
		struct GenericPOData
		{
			size_t DataSize;

			DataTypeSent DataType;
			int ObjectID;

		} GenericData;

		// Only the actual data that had been directly copied so no extra information
		std::vector<char> Data;

		operator const void *() const
		{
			return Data.data();
		}

		static inline int genericPodDataSize = sizeof(NetworkData::PerObjectData::GenericPOData);
	};

	// there should only be one of these in each message
	struct GenericNetorkData
	{
		MessageType messageType;

		int TotalDataSize = 0;
		int DataSegmentsSent = 0;

		bool IsGarantied;
		int NetworkSendID;

		// ATM this will be true if it originated from client and false if it is from server
		int whoSentData;
	} GenericData;

	std::vector<PerObjectData> dataVector;

	NetworkData() = default;
};

struct MultiDataPackage
{
	MessageType messageType;
	int totalDataToBeSent;
	int nameSize;
	int dataPart;
	
	char data[DEFAULT_BUFLEN - 16];
};

struct RecieveDataReturn
{
	RecieveDataReturn() = default;
	RecieveDataReturn(std::vector<NetworkData::PerObjectData> aDataVector) : DataVector(aDataVector) {};

	std::vector<NetworkData::PerObjectData> DataVector;
};

class NetRole
{
public:
	friend class NetworkManager;

	NetRole() = default;
	~NetRole() = default;

	virtual void StartRecieve() {}

	virtual int SendPartData(const void *aData, int aBytesToSend, DataTypeSent aDataSent, int aGOID, bool aIsGaranteed = false, int aPortToSendTo = 0, int aNetworkID = 0) { return 0; };
	virtual int SendData(const void *aData, int aBytesToSend, DataTypeSent aDataSent, int aLobby, int aGOID, bool aIsGaranteed = false, int aPortToSendTo = 0, int aNetworkID = 0) = 0;
	virtual int CommitDataSend() = 0;

	virtual std::optional<RecieveDataReturn> RecieveData() = 0;

	float GetPing() { return myPing; }
	int GetPacketLoss() { return myPacketloss; }
	int GetSentPerSecond() { return mySentPerSecond;}
	int GetRecievedPerSecond() { return myRecievePerSecond;}

		int mySentPerSecond = 0;
	int myRecievePerSecond = 0;

	int myTotalDataSent = 0;
	int myTotalDataRecieved = 0;

	float nextTimeToUpdateSentRec = 0;
	
protected:
	boostIp::udp::endpoint mySenderEndpoint;

	int myRecvBufferlen = DEFAULT_BUFLEN;
	char myRecvBuffer[DEFAULT_BUFLEN];

	float myPing = 0;
	int myPacketloss = 0;
};

#endif