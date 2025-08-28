#pragma once

#ifdef USENET

#include <vector>
#include <thread>
#include <mutex>

#include <Networking/Shared/Common.h>
#include <boost/asio/io_context.hpp>
#include <vector>

class NetRole;
class Component;
class NetRole;
class GameObject;

class NetworkManager
{
public:
	NetworkManager();

	~NetworkManager() = default;

	Component *AddComponentIDX(GameObject *aGameObject, int i);

	void StartGameClient();
	void StartGameServer();

	void RecieveData();

	NetRole *GetNetRole();

	void Update();

	std::mutex networkMutex;
	bool createObject = false;
	GameObject *myGameObject;

	int myLobbyPartOff = -1;
	
	std::vector<int> myLobbiesAvaible;
private:
	boost::asio::io_context myIoContext;
	NetRole *myNetRole;
	std::thread myRecieveDataThread;

	float timeUntilReconnectTry = 0.1f;

	std::vector<unsigned> netIDSToRemove;

};

#endif