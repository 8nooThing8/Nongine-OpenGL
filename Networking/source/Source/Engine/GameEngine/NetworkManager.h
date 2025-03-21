#pragma once

#include <vector>

#include <thread>

#include <unordered_map>

#include <../Networking/Shared/Common.h>

class NetRole;
class Component;
class Client;

class NetworkManager
{
public:
	NetworkManager();

	~NetworkManager() = default;

	void SubscribeMeToNetworkRecieve(Component* aComponent, DataTypeSent aDataType);

	void StartGame();
	
	void RecieveData();

	Client* GetClient();

private:
	bool myIsServer;
	Client* myClient;

	std::thread myRecieveDataThread;

	std::unordered_map<DataTypeSent, std::vector<Component*>> mySubscribers;
};