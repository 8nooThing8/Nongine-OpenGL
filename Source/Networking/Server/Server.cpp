#ifdef USENET

#include "Networking/Server/Server.h"

#include <thread>

#include <iostream>

#include "Vector/Vector2.hpp"
#include "Vector/Vector3.hpp"

#include <sstream>

#include <cassert>

#include "Engine/GameEngine/NetworkManager.h"
#include "Engine/GameEngine/MainSingleton.h"

#include <Timer.h>

#include <iostream>

#include <Vector/Vector2.hpp>
#include "Server.h"

#include "Engine/GameEngine/Components/GameObject.h"

int recievedd = 0;

Server::Server(boostAsio::io_context &aIoContext) : mySocket(boostIp::udp::socket(aIoContext, boostIp::udp::endpoint(boostIp::udp::v4(), DEFAULT_PORT)))
{
	StartRecieve();
}

void Server::SafeAsyncSend(const void *src, std::size_t len, const boostIp::udp::endpoint &endPoint)
{
	mySocket.async_send_to(boost::asio::buffer(src, len), endPoint,
						   boost::bind(&Server::HandleSend, this,
									   myRecvBuffer,
									   boost::asio::placeholders::error,
									   boost::asio::placeholders::bytes_transferred));
}

int Server::SendData(const void *aData, int aBytesToSend, DataTypeSent aDataSent, int aLobby, int aGOID, bool aIsGaranteed, int aPortToSendTo, int aNetworkID)
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

	if (aPortToSendTo != 0)
	{
		if (myPortToSocket.find(aPortToSendTo) != myPortToSocket.end())
		{
			if (aIsGaranteed)
			{
				auto &garantiedNetworkData = myPortToSocket.at(aPortToSendTo).garantiedNetworkData;

				AddMessageIntrnal(static_cast<unsigned>(aPortToSendTo), totalAfterNewData, aBytesToSend, perObjData, garantiedNetworkData);
			}
			else
			{
				auto &nonGarantiedNetworkData = myPortToSocket.at(aPortToSendTo).nonGarantiedNetworkData;

				AddMessageIntrnal(static_cast<unsigned>(aPortToSendTo), totalAfterNewData, aBytesToSend, perObjData, nonGarantiedNetworkData);
			}
		}

		return 0;
	}

	if (aIsGaranteed)
	{
		if (aLobby == -1)
		{
			for (auto &socket : myPortToSocket)
			{
				unsigned port = socket.first;
				auto &garantiedNetworkData = socket.second.garantiedNetworkData;

				AddMessageIntrnal(port, totalAfterNewData, aBytesToSend, perObjData, garantiedNetworkData);
			}
			return 0;
		}
		for (auto &port : myLobbyToPort.at(aLobby))
		{
			auto &portEndpoint = myPortToSocket.at(port);
			auto &garantiedNetworkData = portEndpoint.garantiedNetworkData;

			AddMessageIntrnal(port, totalAfterNewData, aBytesToSend, perObjData, garantiedNetworkData);
		}
		return 0;
	}
	else
	{
		if (aLobby == -1)
		{
			for (auto &socket : myPortToSocket)
			{
				unsigned port = socket.first;
				auto &nonGarantiedNetworkData = socket.second.nonGarantiedNetworkData;

				AddMessageIntrnal(port, totalAfterNewData, aBytesToSend, perObjData, nonGarantiedNetworkData);
			}
			return 0;
		}
		for (auto &port : myLobbyToPort.at(aLobby))
		{
			auto &portEndpoint = myPortToSocket.at(port);
			auto &nonGarantiedNetworkData = portEndpoint.nonGarantiedNetworkData;

			AddMessageIntrnal(port, totalAfterNewData, aBytesToSend, perObjData, nonGarantiedNetworkData);
		}
		return 0;
	}

	return 0;
}

void Server::AddMessageIntrnal(const unsigned &port,
							   int &totalAfterNewData,
							   int aBytesToSend,
							   const NetworkData::PerObjectData &perObjData,
							   NetworkData &aNetworkData)
{
	totalAfterNewData += aNetworkData.GenericData.TotalDataSize;

	if (totalAfterNewData >= 512)
		CommitDataSend();

	aNetworkData.GenericData.TotalDataSize += aBytesToSend;
	aNetworkData.GenericData.TotalDataSize += sizeof(NetworkData::PerObjectData::GenericPOData);
	aNetworkData.GenericData.DataSegmentsSent++;

	aNetworkData.dataVector.emplace_back(perObjData);

	
}

int Server::SendFile(const void *aData, int aBytesToSend, const char *aFileName)
{
	int dataContainerSize = static_cast<int>(sizeof(MultiDataPackage::data));

	float dataContainerSizeFloat = static_cast<float>(dataContainerSize);
	float dataToSendFloat = static_cast<float>(aBytesToSend);

	int packagesToSend = static_cast<int>(ceilf(dataToSendFloat / dataContainerSizeFloat));
	int lastPartExtraData = aBytesToSend % dataContainerSize;

	int fileNameLength = strlen(aFileName);

	MultiDataPackage dataPackage;
	dataPackage.messageType = MessageType::FILEMULTIPACKAGE;

	dataPackage.totalDataToBeSent = aBytesToSend;
	dataPackage.nameSize = fileNameLength;

	for (int i = 0; i < packagesToSend - 1; i++)
	{
		dataPackage.dataPart = i;

		std::memcpy(dataPackage.data, static_cast<const char *>(aData) + (dataContainerSize * dataPackage.dataPart), dataContainerSize);

		for (auto &portEndpoint : myPortToSocket)
		{
			char data[512];

			std::memset(data, 0, 512);
			std::memcpy(data, &dataPackage, DEFAULT_BUFLEN);
			{
				auto &allPacketsSent = portEndpoint.second.totalPacketsSent;

				if (portEndpoint.second.recentPacketLoss >= packetsSampleSize)
					portEndpoint.second.recentPacketLoss--;

				allPacketsSent++;

				int netID = networkID--;
				int port = portEndpoint.first;

				NetroleStorage clientStorage(512, port, netID);
				std::memcpy(clientStorage.dataSent, data, 512);

				myGarantiedMessages.emplace(PortNetID(port, netID), clientStorage);
			}

			SafeAsyncSend(data, DEFAULT_BUFLEN, portEndpoint.second.endPointClient);
		}
	}

	std::memset(dataPackage.data, 0, dataContainerSize);

	dataPackage.dataPart = packagesToSend - 1;
	std::memcpy(dataPackage.data, static_cast<const char *>(aData) + (dataContainerSize * dataPackage.dataPart), lastPartExtraData);
	for (auto &portEndpoint : myPortToSocket)
	{
		char data[512];

		std::memset(data, 0, 512);
		std::memcpy(data, &dataPackage, lastPartExtraData + 16);

		std::memcpy(data + lastPartExtraData + 16, aFileName, fileNameLength);

		{
			auto &allPacketsSent = portEndpoint.second.totalPacketsSent;

			if (portEndpoint.second.recentPacketLoss >= packetsSampleSize)
				portEndpoint.second.recentPacketLoss--;

			allPacketsSent++;

			int netID = networkID--;
			int port = portEndpoint.first;

			NetroleStorage clientStorage(lastPartExtraData, port, netID);
			std::memcpy(clientStorage.dataSent, data, lastPartExtraData);

			myGarantiedMessages.emplace(PortNetID(port, netID), clientStorage);
		}

		SafeAsyncSend(data, lastPartExtraData + 16 + fileNameLength, portEndpoint.second.endPointClient);
	}

	return 0;
}
int Server::SendPartData(const void *aData, int aBytesToSend, DataTypeSent aDataSent, int aGOID, bool aIsGaranteed, int aPortToSendTo, int aNetworkID)
{
	char data[512];
	int offset = 0;

	NetworkData netData;
	netData.GenericData.TotalDataSize = sizeof(NetworkData::GenericNetorkData);
	netData.GenericData.DataSegmentsSent = 1;
	netData.GenericData.IsGarantied = true;
	netData.GenericData.NetworkSendID = aNetworkID ? aNetworkID : networkID--;

	size_t genericStartDataSize = sizeof(NetworkData::GenericNetorkData);
	std::memcpy(data, &netData.GenericData, genericStartDataSize);
	offset += genericStartDataSize;

	NetworkData::PerObjectData perObjData;

	perObjData.GenericData.DataSize = aBytesToSend;
	perObjData.GenericData.ObjectID = aGOID;
	perObjData.GenericData.DataType = aDataSent;

	perObjData.Data.resize(aBytesToSend);
	std::memcpy(perObjData.Data.data(), aData, aBytesToSend);

	netData.dataVector.emplace_back(perObjData);

	for (const auto &netDataPart : netData.dataVector)
	{
		size_t genericDataSize = sizeof(NetworkData::PerObjectData::GenericPOData);
		std::memcpy(data + offset, &netDataPart, genericDataSize);
		offset += genericDataSize;

		std::memcpy(data + offset, netDataPart.Data.data(), netDataPart.Data.size());
		offset += netDataPart.Data.size();
	}

	if (aPortToSendTo != 0)
	{
		const auto &endPoint = myPortToSocket.at(aPortToSendTo).endPointClient;

		SafeAsyncSend(data, offset, endPoint);

		return 0;
	}

	for (auto &portEndpoint : myPortToSocket)
	{
		const auto &endPoint = portEndpoint.second.endPointClient;

		SafeAsyncSend(data, perObjData.GenericData.DataSize, endPoint);
	}

	return 0;
}

int Server::SendDataDirectly(const void *aData, int dataSize, int aPortToSendTo)
{
	if (myPortToSocket.find(aPortToSendTo) == myPortToSocket.end())
		return -1;

	char data[DEFAULT_BUFLEN];

	assert(dataSize <= 512 && ", Trying to send too much data send a bit less in smaller pieces");

	memcpy(data, aData, dataSize);

	int iResult = 0;

	if (aPortToSendTo != 0)
	{
		const auto &endPoint = myPortToSocket.at(aPortToSendTo).endPointClient;

		SafeAsyncSend(data, dataSize, endPoint);

		return 0;
	}

	for (auto &portEndpoint : myPortToSocket)
	{
		const auto &endPoint = portEndpoint.second.endPointClient;

		SafeAsyncSend(data, dataSize, endPoint);
	}

	return 0;
}

int Server::CommitDataSend()
{
	// Garantied message send builder
	for (auto &portEndpoint : myPortToSocket)
	{
		int offset = 0;
		char data[512];

		auto &garantiedNetworkData = portEndpoint.second.garantiedNetworkData;

		if (!garantiedNetworkData.dataVector.empty())
		{
			garantiedNetworkData.GenericData.messageType = MessageType::SinglePackage;
			garantiedNetworkData.GenericData.whoSentData = 0;

			garantiedNetworkData.GenericData.NetworkSendID = networkID--;

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

			auto &allPacketsSent = portEndpoint.second.totalPacketsSent;

			if (portEndpoint.second.recentPacketLoss >= packetsSampleSize)
				portEndpoint.second.recentPacketLoss--;

			allPacketsSent++;

			int netID = garantiedNetworkData.GenericData.NetworkSendID;
			int port = portEndpoint.first;

			NetroleStorage clientStorage(garantiedNetworkData.GenericData.TotalDataSize, port, netID);
			std::memcpy(clientStorage.dataSent, data, garantiedNetworkData.GenericData.TotalDataSize);

			myGarantiedMessages.emplace(PortNetID(port, netID), clientStorage);

			const auto &endPoint = portEndpoint.second.endPointClient;

			int totalAmountToSend = garantiedNetworkData.GenericData.TotalDataSize + sizeof(NetworkData::GenericNetorkData);

			SafeAsyncSend(data, totalAmountToSend, endPoint);
		}

		garantiedNetworkData.dataVector.clear();
		garantiedNetworkData.GenericData.DataSegmentsSent = 0;
		garantiedNetworkData.GenericData.NetworkSendID = 0;
		garantiedNetworkData.GenericData.TotalDataSize = 0;
	}

	// Non Garantied message send builder
	for (auto &portEndpoint : myPortToSocket)
	{
		int offset = 0;
		char data[512];

		auto &nonGarantiedNetworkData = portEndpoint.second.nonGarantiedNetworkData;

		if (!nonGarantiedNetworkData.dataVector.empty())
		{
			nonGarantiedNetworkData.GenericData.whoSentData = 0;
			nonGarantiedNetworkData.GenericData.messageType = MessageType::SinglePackage;

			nonGarantiedNetworkData.GenericData.NetworkSendID = networkID--;

			size_t genericStartDataSize = sizeof(NetworkData::GenericNetorkData);
			std::memcpy(data, &nonGarantiedNetworkData.GenericData, genericStartDataSize);
			offset += genericStartDataSize;

			for (const auto &netData : nonGarantiedNetworkData.dataVector)
			{
				size_t genericDataSize = sizeof(NetworkData::PerObjectData::GenericPOData);
				std::memcpy(data + offset, &netData, genericDataSize);
				offset += genericDataSize;

				int sentDataSize = netData.GenericData.DataSize;

				if (netData.Data.size() != sentDataSize)
					continue;

				std::memcpy(data + offset, netData.Data.data(), sentDataSize);
				offset += sentDataSize;
			}

			auto &allPacketsSent = portEndpoint.second.totalPacketsSent;

			if (portEndpoint.second.recentPacketLoss >= packetsSampleSize)
				portEndpoint.second.recentPacketLoss--;

			allPacketsSent++;

			const auto &endPoint = portEndpoint.second.endPointClient;

			int totalAmountToSend = nonGarantiedNetworkData.GenericData.TotalDataSize + sizeof(NetworkData::GenericNetorkData);

			SafeAsyncSend(data, totalAmountToSend, endPoint);
		}

		nonGarantiedNetworkData.dataVector.clear();
		nonGarantiedNetworkData.GenericData.DataSegmentsSent = 0;
		nonGarantiedNetworkData.GenericData.NetworkSendID = 0;
		nonGarantiedNetworkData.GenericData.TotalDataSize = 0;
	}

	return 0;
}

int Server::ReRouteData(const void *aData, int aBytesToSend, int aLobby, int *portsToIgnore, int /*aCountPortToIgnore*/)
{
	for (auto &portEndpoint : myPortToSocket)
	{
		if (portEndpoint.second.port == *portsToIgnore)
			continue;

		// auto &portEndpoint = myPortToSocket.at(port);

		const auto &endPoint = portEndpoint.second.endPointClient;

		SafeAsyncSend(aData, aBytesToSend, endPoint);
	}

	return 0;
}

int Server::ReRouteDataManager(const NetworkData::PerObjectData &aPOD, const NetworkData::GenericNetorkData &aGND)
{
	std::vector<DataTypeSent> dataTypesToReroute = {DataTypeSent::TRANSFORM, DataTypeSent::CHANGETEXTURE, DataTypeSent::CREATEPLAYER};

	bool continueWithReroute = false;

	for (auto &dataType : dataTypesToReroute)
	{
		if (dataType == aPOD.GenericData.DataType)
			continueWithReroute = true;
	}

	if (!continueWithReroute)
		return 0;

	char data[512];
	int offset = 0;
	int totalDataToSend = 0;

	NetworkData::GenericNetorkData newNetworkData;

	newNetworkData.DataSegmentsSent = 1;
	newNetworkData.IsGarantied = aGND.IsGarantied;
	newNetworkData.NetworkSendID = aGND.NetworkSendID;
	newNetworkData.TotalDataSize = aPOD.GenericData.DataSize;

	size_t genericStartDataSize = sizeof(NetworkData::GenericNetorkData);
	std::memcpy(data, &newNetworkData, genericStartDataSize);
	offset += genericStartDataSize;
	totalDataToSend += genericStartDataSize;

	size_t genericDataSize = sizeof(NetworkData::PerObjectData::GenericPOData);
	std::memcpy(data + offset, &aPOD, genericDataSize);
	offset += genericDataSize;
	totalDataToSend += genericDataSize;

	int sentDataSize = aPOD.GenericData.DataSize;
	std::memcpy(data + offset, aPOD.Data.data(), sentDataSize);
	totalDataToSend += sentDataSize;

	int port = mySenderEndpoint.port();

	for (auto &dataType : dataTypesToReroute)
	{
		if (dataType == aPOD.GenericData.DataType)
			ReRouteData(data, totalDataToSend, myPortToSocket.at(port).myLobby, &port, 1);
	}

	return 0;
}

void Server::StartRecieve()
{
	// std::memset(myRecvBuffer, 0, 512);

	mySocket.async_receive_from(
		boost::asio::buffer(myRecvBuffer, 512), mySenderEndpoint,
		boost::bind(&Server::HandleRecieve, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
}

void Server::HandleSend(char message[512], const boost::system::error_code &error, std::size_t aBytesTransferred)
{
	myTotalDataSent++;

	message;
	error;
	aBytesTransferred;
}

bool Server::HandleGarantied(int aPort, int aNetID)
{
	PortNetID portNetID = PortNetID(aPort, aNetID);

	if (myPortToSocket.find(aPort) == myPortToSocket.end())
		return false;

	if (myGarantiedMessages.find(portNetID) == myGarantiedMessages.end())
		return false;

	auto &message = myGarantiedMessages.at(portNetID);

	auto &portToSocket = myPortToSocket.at(aPort);

	auto &pingQueue = portToSocket.pingQueue;

	float ping = CommonUtilities::Timer::GetTotalTime() - message.timeForPing;
	pingQueue.emplace_back(ping);

	float avragePing = 0;

	for (auto &pingValue : pingQueue)
		avragePing += pingValue;

	avragePing /= pingQueue.size();

	if (pingQueue.size() >= pingSampleSize)
	{
		for (int i = 0; i < pingQueue.size() - 1; i++)
			pingQueue[i] = pingQueue[i + 1];

		pingQueue.pop_back();
	}

	myPortToSocket.at(aPort).ping = avragePing;

	myGarantiedMessagesToDelete.emplace_back(portNetID);

	return true;
}

void Server::HandleData(const NetworkData::PerObjectData &aData)
{
	int port = mySenderEndpoint.port();

	if (myPortToSocket.find(port) == myPortToSocket.end())
		HandleNewClient(mySenderEndpoint, 0);

	switch (aData.GenericData.DataType)
	{
	case DataTypeSent::MOVEPLAYEROBJECT:
	{
		CommonUtilities::Vector2f position;
		std::memcpy(&position, aData, sizeof(position));
		myPortToSocket.at(port).clientPlayerPosition = position;
	}
	break;
	case DataTypeSent::JOINLOBBY:
	{
		int lobbyToJoin = 0;
		std::memcpy(&lobbyToJoin, aData, sizeof(lobbyToJoin));

		myPortToSocket.at(port).isInLobby = true;
		myPortToSocket.at(port).myLobby = lobbyToJoin;

		myLobbyToPort[lobbyToJoin].emplace_back(port);

		for (auto &[id, gameobject] : MainSingleton::Get().myGameObjects)
		{
			int _ = 0;
			SendData(&_, 1, DataTypeSent::CREATEGAMEOBJECT, -1, gameobject->GetRawID());
		}
	}
	break;
	default:
		break;
	}
}

void Server::HandleRecieve(const boost::system::error_code &aError, size_t aSizeBytes)
{
	int port = mySenderEndpoint.port();

	if (aError)
	{
		KickClient(port);

		StartRecieve();
		return;
	}

	myTotalDataRecieved++;

	int offset = 0;

	NetworkData networkData;

	std::memcpy(&networkData, myRecvBuffer, sizeof(NetworkData::GenericNetorkData));
	networkData.dataVector.resize(networkData.GenericData.DataSegmentsSent);
	offset = sizeof(NetworkData::GenericNetorkData);

	for (auto &value : networkData.dataVector)
	{
		int genericPOSize = NetworkData::PerObjectData::genericPodDataSize;
		std::memcpy(&value.GenericData, myRecvBuffer + offset, genericPOSize);
		offset += genericPOSize;

		int dataSize = value.GenericData.DataSize;
		value.Data.resize(dataSize);
		std::memcpy(value.Data.data(), myRecvBuffer + offset, dataSize);

		offset += dataSize;
	}

	StartRecieve();

	for (auto &data : networkData.dataVector)
	{
		HandleData(data);
		ReRouteDataManager(data, networkData.GenericData);

		int netID = networkData.GenericData.NetworkSendID;

		if (networkData.GenericData.IsGarantied)
		{
			if (HandleGarantied(port, netID))
				return;

			switch (data.GenericData.DataType)
			{
			case DataTypeSent::CONNECTIONINFOCLIENT:
			{
				if (myPortToSocket.find(port) == myPortToSocket.end())
					return;

				// 0: Ping
				// 1: Packet loss
				float information[2];

				float ping = myPortToSocket.at(port).ping;
				information[0] = ping;

				auto &portToSocket = myPortToSocket.at(port);
				int recentPackageLoss = portToSocket.recentPacketLoss;
				int totalPackages = portToSocket.totalPacketsSent;
				float recentPackageLossFloat = static_cast<float>(recentPackageLoss);
				float totalPackagesFloat = static_cast<float>(totalPackages);
				float packageLoss = recentPackageLossFloat / totalPackagesFloat;

				information[1] = packageLoss;

				SendPartData(&information, sizeof(information), data.GenericData.DataType, data.GenericData.ObjectID, true, port, netID);
			}
			break;
			case DataTypeSent::ASKAVAIBLELOBBIES:
			{
				char Data[512];
				LobbyListAnswer lobbyList;

				lobbyList.totalLobbies = myLobbyToPort.size();

				for (auto &lobby : myLobbyToPort)
					lobbyList.lobbiesAvaible.emplace_back(lobby.first);

				int offset = 0;
				int dataToCopy = 0;

				dataToCopy = sizeof(lobbyList.totalLobbies);
				std::memcpy(Data, &lobbyList.totalLobbies, dataToCopy);
				offset += dataToCopy;

				dataToCopy = sizeof(lobbyList.totalLobbies) * lobbyList.lobbiesAvaible.size();
				std::memcpy(Data + offset, lobbyList.lobbiesAvaible.data(), dataToCopy);
				offset += dataToCopy;

				SendPartData(Data, offset, data.GenericData.DataType, data.GenericData.ObjectID, true, port, netID);
			}
			break;
			default:
			{
				SendDataDirectly(myRecvBuffer, aSizeBytes, port);
			}
			break;
			}
		}
	}
}

void Server::HandleNewClient(const boostIp::udp::endpoint &aEndpoint, unsigned aLobby)
{
	int port = aEndpoint.port();

	ClientStorage storage(aEndpoint, port, 0, aLobby);
	storage.isInLobby = false;

	storage.garantiedNetworkData.GenericData.IsGarantied = true;
	storage.nonGarantiedNetworkData.GenericData.IsGarantied = false;

	myPortToSocket.emplace(port, storage);

	std::cout << ("new client connected with port %i", port) << std::endl;
}

void Server::KickClient(int aPort)
{
	myPortsToKick.emplace_back(aPort);
}

void Server::KickClients()
{
	std::vector<PortNetID> netIDSToRemove;

	for (auto port : myPortsToKick)
	{
		std::cout << "Kicked client with port: " << port << std::endl;

		// Incase anyone is is listening so they can connect quick back without having to leave the game because of this
		SendData(&port, 1, DataTypeSent::KICK, 0, 0, false, port);

		for (const auto &storage : myGarantiedMessages)
			if (storage.first.port == port)
				netIDSToRemove.emplace_back(storage.first);

		if (myPortToSocket.find(port) == myPortToSocket.end())
			continue;

		auto &lobbyPort = myLobbyToPort.at(myPortToSocket.at(port).myLobby);
		for (int i = 0; i < lobbyPort.size(); i++)
		{
			if (lobbyPort[i] == port)
			{
				lobbyPort[i] = lobbyPort.back();
				lobbyPort.pop_back();
			}
		}

		myPortToSocket.erase(port);
	}

	for (const auto &id : netIDSToRemove)
		myGarantiedMessages.erase(id);

	myPortsToKick.clear();
}

std::optional<RecieveDataReturn> Server::RecieveData()
{
	return std::nullopt;
}

void Server::ConnectNewClient()
{
}

void Server::DeleteMessages()
{
	for (int i = 0; i < myGarantiedMessagesToDelete.size(); i++)
		myGarantiedMessages.erase(myGarantiedMessagesToDelete[i]);

	myGarantiedMessagesToDelete.clear();
}
#endif