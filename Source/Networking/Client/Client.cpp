#ifdef USENET
#include "client.h"

#include <iostream>

#include "Vector/Vector2.hpp"
#include "Vector/Vector3.hpp"

#include <cassert>
#include <String.hpp>

#include <Timer.h>
#include "Client.h"

#include <fstream>

#include <thread>

Client::Client(boostAsio::io_context &aIoContext) : mySocket(aIoContext, boostIp::udp::v4())
{
	boostIp::udp::resolver resolver(aIoContext);
	myEndpoint = *resolver.resolve(boostIp::udp::v4(), "127.0.0.1", std::to_string(DEFAULT_PORT)).begin();

	garantiedNetworkData.GenericData.IsGarantied = true;
	nonGarantiedNetworkData.GenericData.IsGarantied = false;
}

int Client::CommitDataSend()
{
	if (nonGarantiedNetworkData.dataVector.empty() && garantiedNetworkData.dataVector.empty())
		return 0;

	// assert(aBytesToSend < 512 - sizeof(SentData) && ", data type to send is set to none on the component im trying to send");

	// Garantied message send builder
	{
		char data[512];

		int offset = 0;

		if (!garantiedNetworkData.dataVector.empty())
		{
			garantiedNetworkData.GenericData.whoSentData = 1;
			garantiedNetworkData.GenericData.messageType = MessageType::SinglePackage;

			garantiedNetworkData.GenericData.NetworkSendID = networkID++;

			size_t genericStartDataSize = sizeof(NetworkData::GenericNetorkData);
			std::memcpy(data, &garantiedNetworkData.GenericData, genericStartDataSize);
			offset += genericStartDataSize;
				
			for (const auto &netData : garantiedNetworkData.dataVector)
			{
				size_t genericDataSize = sizeof(NetworkData::PerObjectData::GenericPOData);
				std::memcpy(data + offset, &netData, genericDataSize);
				offset += genericDataSize;

				int sentDataSize = netData.GenericData.DataSize;
				std::memcpy(data + offset, netData.Data.data(), sentDataSize);
				offset += sentDataSize;
			}

			int port = myEndpoint.port();

			NetroleStorage storage(garantiedNetworkData.GenericData.TotalDataSize, port, garantiedNetworkData.GenericData.NetworkSendID);

			std::memcpy(storage.dataSent, data, garantiedNetworkData.GenericData.TotalDataSize);
			storage.typeSent = garantiedNetworkData.dataVector[0].GenericData.DataType;

			// if (aGOID == -1)
			// 	storage.currentTimeoutCount = storage.timesUntilTimeout;

			int netID = garantiedNetworkData.GenericData.NetworkSendID;

			myPortsToEndpoint.emplace(netID, myEndpoint);
			myPortsToNetroleStorage.emplace(netID, storage);

			int iResult = 0;
			int totalAmountToSend = garantiedNetworkData.GenericData.TotalDataSize + sizeof(NetworkData::GenericNetorkData);
			iResult = mySocket.send_to(boost::asio::buffer(data, totalAmountToSend), myEndpoint);

			myTotalDataSent++;
		}
	}

	// Non Garantied message send builder
	{
		char data[512];

		int offset = 0;

		if (!nonGarantiedNetworkData.dataVector.empty())
		{
			nonGarantiedNetworkData.GenericData.whoSentData = 1;
			nonGarantiedNetworkData.GenericData.messageType = MessageType::SinglePackage;

			nonGarantiedNetworkData.GenericData.NetworkSendID = networkID++;

			size_t genericStartDataSize = sizeof(NetworkData::GenericNetorkData);
			std::memcpy(data, &nonGarantiedNetworkData.GenericData, genericStartDataSize);
			offset += genericStartDataSize;

			for (const auto &netData : nonGarantiedNetworkData.dataVector)
			{
				size_t genericDataSize = sizeof(NetworkData::PerObjectData::GenericPOData);
				std::memcpy(data + offset, &netData, genericDataSize);
				offset += genericDataSize;

				int sentDataSize = netData.GenericData.DataSize;
				std::memcpy(data + offset, netData.Data.data(), sentDataSize);
				offset += sentDataSize;
			}

			int iResult = 0;
			int totalAmountToSend = nonGarantiedNetworkData.GenericData.TotalDataSize + sizeof(NetworkData::GenericNetorkData);
			iResult = mySocket.send_to(boost::asio::buffer(data, totalAmountToSend), myEndpoint);

			myTotalDataSent++;
		}
	}

	nonGarantiedNetworkData.dataVector.clear();
	nonGarantiedNetworkData.GenericData.DataSegmentsSent = 0;
	nonGarantiedNetworkData.GenericData.NetworkSendID = 0;
	nonGarantiedNetworkData.GenericData.TotalDataSize = 0;

	garantiedNetworkData.dataVector.clear();
	garantiedNetworkData.GenericData.DataSegmentsSent = 0;
	garantiedNetworkData.GenericData.NetworkSendID = 0;
	garantiedNetworkData.GenericData.TotalDataSize = 0;

	return 0;
}

void Client::HandleFileData(const char *aData, int dataCount)
{
	MultiDataPackage multiDataPacket;
	std::memcpy(&multiDataPacket, aData, dataCount);

	if (!isMakingBigMessage)
	{
		isMakingBigMessage = true;
		myBigMessage = static_cast<char *>(std::malloc(multiDataPacket.totalDataToBeSent + multiDataPacket.nameSize));
	}

	std::memcpy(myBigMessage + (sizeof(MultiDataPackage::data) * multiDataPacket.dataPart), multiDataPacket.data, dataCount - 16);

	totalDataInBigMessage += dataCount - 16;

	if (totalDataInBigMessage >= multiDataPacket.totalDataToBeSent + multiDataPacket.nameSize)
	{
		int nameLength = multiDataPacket.nameSize;

		std::vector<char> snapshotData(myBigMessage, myBigMessage + totalDataInBigMessage);

		isMakingBigMessage = false;
		totalDataInBigMessage = 0;

		char *nameChar = static_cast<char *>(std::malloc(nameLength));
		std::memcpy(nameChar, snapshotData.data() + multiDataPacket.totalDataToBeSent, nameLength);
		std::memcpy(nameChar + nameLength, "\0", 1);

		std::ofstream out(nameChar, std::ios::binary);
		out.write(snapshotData.data(), static_cast<std::streamsize>(snapshotData.size() - nameLength));
		out.flush();
		out.close();

		delete myBigMessage;
		delete nameChar;
	}
}

int Client::SendDataDirectly(const void *aData, int dataSize, int aPortToSendTo)
{
	if (dataSize > 511)
		return 0;

	char data[DEFAULT_BUFLEN];

	assert(dataSize < 512 && ", Trying to send too much data send a bit less in smaller pieces");

	memcpy(data, aData, dataSize);

	int iResult = 0;

	iResult = mySocket.send_to(boost::asio::buffer(data, dataSize), myEndpoint);

	myTotalDataSent++;

	return iResult;
}

std::optional<RecieveDataReturn> Client::RecieveData()
{
	int iResult;

	sockaddr_in senderAddr;
	int senderAddrSize = sizeof(senderAddr);

	boost::system::error_code errorCode;

	std::memset(myRecvBuffer, 0, 512);

	iResult = mySocket.receive_from(boost::asio::buffer(myRecvBuffer, 512), mySenderEndpoint, 0, errorCode);
	if (errorCode)
		return std::nullopt;

	myTotalDataRecieved++;

	if (iResult > 0)
	{
		int offset = 0;

		NetworkData networkData;

		std::memcpy(&networkData, myRecvBuffer, sizeof(NetworkData::GenericNetorkData));

		if (networkData.GenericData.messageType == MessageType::FILEMULTIPACKAGE)
		{
			HandleFileData(myRecvBuffer, iResult);
			return std::nullopt;
		}

		int netID = networkData.GenericData.NetworkSendID;

		networkData.dataVector.resize(networkData.GenericData.DataSegmentsSent);
		offset = sizeof(NetworkData::GenericNetorkData);

		for (int i = 0; i < networkData.dataVector.size(); i++)
		{
			int genericPOSize = NetworkData::PerObjectData::genericPodDataSize;
			std::memcpy(&networkData.dataVector[i].GenericData, myRecvBuffer + offset, genericPOSize);
			offset += genericPOSize;

			int dataSize = networkData.dataVector[i].GenericData.DataSize;
			if (dataSize > 512)
			{
				networkData.dataVector[i] = NetworkData::PerObjectData();
				networkData.dataVector.pop_back();

				continue;
			}

			networkData.dataVector[i].Data.resize(dataSize);
			std::memcpy(networkData.dataVector[i].Data.data(), myRecvBuffer + offset, dataSize);
			offset += dataSize;
		}

		if (myPortsToEndpoint.find(netID) != myPortsToEndpoint.end())
		{
			myPortsToEndpoint.erase(netID);
			myPortsToNetroleStorage.erase(netID);
		}
		else if (networkData.GenericData.whoSentData == 0 && networkData.GenericData.IsGarantied)
		{
			SendDataDirectly(myRecvBuffer, iResult, 0);
		}

		RecieveDataReturn dataReturn(networkData.dataVector);

		return dataReturn;
	}

	return std::nullopt;
}

int Client::SendData(const void *aData, int aBytesToSend, DataTypeSent aDataSent, int aLobby, int aGOID, bool aIsGaranteed, int aPortToSendTo, int aNetworkID)
{
	NetworkData::PerObjectData perObjData;

	perObjData.GenericData.DataSize = aBytesToSend;
	perObjData.GenericData.ObjectID = aGOID;
	perObjData.GenericData.DataType = aDataSent;

	perObjData.Data.resize(aBytesToSend);
	std::memcpy(perObjData.Data.data(), aData, aBytesToSend);

	int genericNetworkData = sizeof(NetworkData::GenericNetorkData);
	int genericPOData = sizeof(NetworkData::PerObjectData::GenericPOData);

	int totalAfterNewData = aBytesToSend + genericNetworkData + genericPOData;

	if (aIsGaranteed)
	{
		totalAfterNewData += garantiedNetworkData.GenericData.TotalDataSize;

		if(totalAfterNewData)
			CommitDataSend();

		garantiedNetworkData.GenericData.TotalDataSize += aBytesToSend;
		garantiedNetworkData.GenericData.TotalDataSize += sizeof(NetworkData::PerObjectData::GenericPOData);
		garantiedNetworkData.GenericData.DataSegmentsSent++;

		garantiedNetworkData.dataVector.emplace_back(perObjData);
	}
	else
	{
		totalAfterNewData += nonGarantiedNetworkData.GenericData.TotalDataSize;

		if(totalAfterNewData)
			CommitDataSend();

		nonGarantiedNetworkData.GenericData.TotalDataSize += aBytesToSend;
		nonGarantiedNetworkData.GenericData.TotalDataSize += sizeof(NetworkData::PerObjectData::GenericPOData);
		nonGarantiedNetworkData.GenericData.DataSegmentsSent++;

		nonGarantiedNetworkData.dataVector.emplace_back(perObjData);
	}

	return 0;
}
#endif