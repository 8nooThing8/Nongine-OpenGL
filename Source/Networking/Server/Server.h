#pragma once

#ifdef USENET

#include <string>

#include "../Shared/NetRole.h"

#include <memory>
#include <unordered_map>
#include <vector>
#include <Vector/Vector2.hpp>

#include <set>

struct ClientStorage
{
	NetworkData nonGarantiedNetworkData;
	NetworkData garantiedNetworkData;

	boostIp::udp::endpoint endPointClient;
	int port;
	float ping;
	int totalPacketsSent = 0;
	int recentPacketLoss = 0;

	bool isInLobby;
	int myLobby = 0;

	std::vector<float> pingQueue;

	CommonUtilities::Vector2f clientPlayerPosition;

	ClientStorage(const boostIp::udp::endpoint &aEndPoint, int aPort, float aPing, int aLobby) : endPointClient(aEndPoint), port(aPort), ping(aPing), myLobby(aLobby)
	{
	}
};

struct PortNetID
{
	unsigned port;
	unsigned networkID;

	PortNetID(unsigned aPort, unsigned aNetworkID) : port(aPort), networkID(aNetworkID)
	{
	}
};

struct KeyHash
{
	std::size_t operator()(const PortNetID &k) const
	{
		return std::hash<unsigned>()(k.port) ^ (std::hash<unsigned>()(k.networkID) << 1);
	}
};

struct KeyEqual
{
	bool operator()(const PortNetID &lhs, const PortNetID &rhs) const
	{
		return lhs.port == rhs.port && lhs.networkID == rhs.networkID;
	}
};

struct LobbyListAnswer
{
	int totalLobbies;
	std::vector<int> lobbiesAvaible;
};

class Server : public NetRole
{
public:
	Server(boostAsio::io_context &aIoContext);
    ~Server() = default;
	
    void SafeAsyncSend(const void *src, std::size_t len, const boostIp::udp::endpoint &endPoint);

    int SendPartData(const void *aData, int aBytesToSend, DataTypeSent aDataSent, int aGOID, bool aIsGaranteed = false, int aPortToSendTo = 0, int aNetworkID = 0) override;
	int SendData(const void *aData, int aBytesToSend, DataTypeSent aDataSent, int aLobby, int aGOID, bool aIsGaranteed = false, int aPortToSendTo = 0, int aNetworkID = 0) override;

	int SendFile(const void *aData, int aBytesToSend, const char *aFileName);

	int SendDataDirectly(const void *aData, int dataSize, int aPortToSendTo = 0);

	int CommitDataSend() override;

	int ReRouteData(const void *aData, int aBytesToSend, int aLobby, int *portsToIgnore, int aCountPortToIgnore);

	int ReRouteDataManager(const NetworkData::PerObjectData &aPOD, const NetworkData::GenericNetorkData &aGND);

	void StartRecieve() override;

	void HandleSend(char message[512], const boost::system::error_code &error, std::size_t aBytesTransferred);
	void HandleNewClient(const boostIp::udp::endpoint &aEndpoint, unsigned aLobby);

	// Check if message was garantied
	bool HandleGarantied(int aPort, int aNetID);
	void HandleRecieve(const boost::system::error_code &aError, size_t aSizeBytes);
	void HandleData(const NetworkData::PerObjectData &aData);

	std::optional<RecieveDataReturn> RecieveData() override;

	void ConnectNewClient();

	void KickClient(int aPort);
	void KickClients();

	std::unordered_map<PortNetID, NetroleStorage, KeyHash, KeyEqual> &GetGarantiedMessages() { return myGarantiedMessages; }
	std::unordered_map<unsigned, ClientStorage> &GetPortToSockAddr() { return myPortToSocket; }
	void DeleteMessages();

public:
	static inline float myAwarnessCircle = 0.5f;

	std::unordered_map<unsigned, std::vector<unsigned>> myLobbyToPort;

private:
	void AddMessageIntrnal(const unsigned& port,
                               int& totalAfterNewData,
                               int aBytesToSend,
                               const NetworkData::PerObjectData& perObjData,
                               NetworkData& aNetworkData);
private:
	std::vector<unsigned> myPortsToKick;

	boostIp::udp::socket mySocket;

	std::unordered_map<PortNetID, NetroleStorage, KeyHash, KeyEqual> myGarantiedMessages;
	std::vector<PortNetID> myGarantiedMessagesToDelete;

	std::unordered_map<unsigned, ClientStorage> myPortToSocket;

	int clientsConnected;

	unsigned networkID = INT32_MAX;

	int packetsSampleSize = 10;
	int pingSampleSize = 10;
};

#endif