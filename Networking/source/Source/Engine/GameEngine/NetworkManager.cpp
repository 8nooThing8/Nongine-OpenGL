#include "NetworkManager.h"

#include "../Engine/GameEngine/Components/Component.h"
#include "../Engine/GameEngine/MainSingleton.h"
#include "../Engine/GameEngine/SceneManagerInternal.h"

#include "../Networking/Client/Client.h"
#include "../Networking/Server/Server.h"

NetworkManager::NetworkManager() : myIsServer(false), myClient(nullptr)
{

}

void NetworkManager::SubscribeMeToNetworkRecieve(Component* aComponent, DataTypeSent aDataType)
{
	mySubscribers[aDataType].emplace_back(aComponent);
}

void NetworkManager::StartGame()
{
	myClient = new Client();
	int iResult = myClient->Init();
	iResult;
	//if (iResult != 0)
	//{
	//	delete (Client*)myNetRole;
	//	myNetRole = new Server();

	//	std::cout << "Waiting for first client to conect..." << std::endl;

	//	iResult = myNetRole->Init();

	//	std::cout << "Started server succesfully" << std::endl;

	//	myIsServer = true;
	//}
	//else
	//{
	//	myIsServer = false;
	//}

	myIsServer = false;

	myRecieveDataThread = std::thread(&NetworkManager::RecieveData, this);
	myRecieveDataThread.detach();
}

void NetworkManager::RecieveData()
{
	while (true)
	{
		NetworkRecieveData networkData = myClient->RecieveData();

		if (!myIsServer)
		{
			//switch (networkData.dataType)
			//{
			//case DataTypeSent::TRANSFORM:
			//	//MainSingleton::Get()->activeScene->gameObjects[]
			//	break;
			//}

			auto myDataTypeSubscriber = mySubscribers.at(networkData.dataType);

			for (auto subscriber : myDataTypeSubscriber)
				subscriber->RecieveDataNetwork(networkData);
		}
	}
}

Client* NetworkManager::GetClient()
{
	return myClient;
}
