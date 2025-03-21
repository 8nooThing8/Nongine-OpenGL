#pragma once

#include <vector>

#include <thread>

#include <unordered_map>

#include <../Networking/Shared/Common.h>

#include <mutex>

class NetRole;
class Component;
class Client;

class NetworkManager
{
public:
	NetworkManager();

	~NetworkManager() = default;

	void StartGame();
	
	void RecieveData();

	Client* GetClient();

	std::mutex networkMutex;

private:
	Client* myClient;

	std::thread myRecieveDataThread;
};