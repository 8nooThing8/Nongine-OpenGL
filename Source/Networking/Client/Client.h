#pragma once
#ifdef USENET
#include "../Shared/NetRole.h"

#include <unordered_map>

#include <queue>

class Client : public NetRole
{
public:
	Client(boostAsio::io_context& aIoContext);
	~Client() = default;

	int SendDataDirectly(const void *aData, int dataSize, int aPortToSendTo);

	int CommitDataSend() override;

	void HandleFileData(const char* aData, int dataCount);

	std::optional<RecieveDataReturn> RecieveData() override;

	int SendData(const void *aData, int aBytesToSend, DataTypeSent aDataSent, int aLobby, int aGOID, bool aIsGaranteed = false, int aPortToSendTo = 0, int aNetworkID = 0) override;

	bool IsConnected() { return isConnected; }
	void IsConnected(bool aConnected) { isConnected = aConnected; }

	std::unordered_map<int, boostIp::udp::endpoint>& GetPortsToEndpoint() { return myPortsToEndpoint; }
	std::unordered_map<int, NetroleStorage>& GetNetStorage() { return myPortsToNetroleStorage; }

private:
	bool isConnected = false;
	float timeSinceLastPingcheck = 0;

	boostIp::udp::socket mySocket;
	boostIp::udp::endpoint myEndpoint;

	std::unordered_map<int, boostIp::udp::endpoint> myPortsToEndpoint;
	std::unordered_map<int, NetroleStorage> myPortsToNetroleStorage;

	unsigned networkID = 1;

	NetworkData nonGarantiedNetworkData;
	NetworkData garantiedNetworkData;

	bool isMakingBigMessage = false;
	char* myBigMessage = nullptr;
	int totalDataInBigMessage = 0;
};
#endif