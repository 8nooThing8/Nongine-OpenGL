#ifdef USENET

#include "NetworkManager.h"

#include "../Engine/GameEngine/Components/Sprite2DRenderer.h"

#include "../Engine/GameEngine/Components/Component.h"
#include "../Engine/GameEngine/Components/Networking/NetworkComponent.h"
#include "../Engine/GameEngine/MainSingleton.h"

#include "../Networking/Client/Client.h"
#include "../Networking/Server/Server.h"
#include "../Networking/Shared/NetRole.h"

#include "../Engine/GameEngine/Components/GameObject.h"

#include "../Engine/GameEngine/Components/MeshRenderer.h"

#include "../Engine/GameEngine/Components/Networking/NetworkTransform.h"

#include <rapidjson-master/include/rapidjson/rapidjson.h>
#include <rapidjson-master/include/rapidjson/document.h>
#include <rapidjson-master/include/rapidjson/filereadstream.h>

#include <optional>

#include "Hash.h"
#include "GraphicsEngine.h"
#include "Application/editor.h"

#include "Engine/GraphicsEngine/Material/Material.h"

#include "Components/Physics2D/Colliders/BoxCollider2D.h"

#include <GLFW/glfw3.h>

#include <Timer.h>

#include <iostream>

NetworkManager::NetworkManager() : myNetRole(nullptr)
{
}

// 0: Sprite2DRenderer

Component *NetworkManager::AddComponentIDX(GameObject *aGameObject, int i)
{
	switch (i)
	{
	case 0:
		return aGameObject->AddComponent<Sprite2DRenderer>();
	}

	return nullptr;
}

void NetworkManager::StartGameClient()
{
	myNetRole = new Client(myIoContext);
	myIoContext.run();

	myRecieveDataThread = std::thread(&NetworkManager::RecieveData, this);
	myRecieveDataThread.detach();
}

void NetworkManager::StartGameServer()
{
	myNetRole = new Server(myIoContext);
	myIoContext.run();

	/*myRecieveDataThread = std::thread(&NetworkManager::RecieveData, this);
	myRecieveDataThread.detach();*/
}

void NetworkManager::Update()
{
	if (myNetRole->nextTimeToUpdateSentRec <= 0.f)
	{
		myNetRole->nextTimeToUpdateSentRec = 1.f;

		myNetRole->mySentPerSecond = myNetRole->myTotalDataSent;
		myNetRole->myRecievePerSecond = myNetRole->myTotalDataRecieved;

		myNetRole->myTotalDataRecieved = 0;
		myNetRole->myTotalDataSent = 0;
	}
	else
	{
		myNetRole->nextTimeToUpdateSentRec -= CommonUtilities::Timer().GetDeltaTime();
	}

	float totalTime = CommonUtilities::Timer::GetTotalTime();
	Client *client = static_cast<Client *>(myNetRole);
	auto &netStorage = client->GetNetStorage();

	for (auto &netrole : netStorage)
	{
		if (totalTime >= netrole.second.timeAtSend + 0.3f)
		{
			if (netrole.second.currentTimeoutCount >= netrole.second.timesUntilTimeout)
			{
				std::cout << "Not connected trying to reconnect" << std::endl;

				netIDSToRemove.emplace_back(netrole.second.networkID);

				client->IsConnected(false);
			}

			reinterpret_cast<Client *>(MainSingleton::Get().myNetworkManager->GetNetRole())->SendDataDirectly(netrole.second.dataSent, netrole.second.dataSize, 0);
			netrole.second.currentTimeoutCount++;
		}
	}

	for (auto &netID : netIDSToRemove)
	{
		netStorage.erase(netID);
	}

	netIDSToRemove.clear();
}

void CreateNetworkObject(NetworkData::PerObjectData &aDataObj, std::unordered_map<int, GameObject *> &aGameobjects)
{
	MainSingleton::Get().myNetworkQueue.emplace([aDataObj, &aGameobjects]()
												{
					GameObject *go = new GameObject("ServerObject");
					go->SetID(aDataObj.GenericData.ObjectID);

					go->transform->SetScale(CU::Vector4f(0.1f, 0.1f, 0.1f, 1));
					go->transform->SetPosition(CU::Vector4f(0.f, -0.2f, 0.f, 1));

					Material mat("Assets/Shaders/DefaultSpriteFS.glsl", "Assets/Shaders/DefaultSpriteVS.glsl");
					auto spriteRenderer = go->AddComponent<Sprite2DRenderer>(mat, 0);

					spriteRenderer->GetMaterial().SetTexture("Assets/Textures/Tex.png", DIFFUSE, SamplingType::Point);

					auto netTransform = go->AddComponent<NetworkTransform>();
					netTransform->sendData = false;

					//go->AddComponent<non::BoxCollider2D>(CU::Vector2f{0.05f, 0.05f});

					go->StartMyComponents();
					aGameobjects.emplace(aDataObj.GenericData.ObjectID, go); });
}

void NetworkManager::RecieveData()
{
	int val = 1;
	Client *client = static_cast<Client *>(myNetRole);

	std::cout << "Connecting to server" << std::endl;

	while (true)
	{
		std::optional<RecieveDataReturn> networkData = client->RecieveData();

		int dataOffet = 0;

		if (!networkData.has_value())
			continue;

		auto &unreferencedValue = networkData.value();

		for (auto &dataObj : unreferencedValue.DataVector)
		{
			int goID = dataObj.GenericData.ObjectID;

			auto &gameobjects = MainSingleton::Get().myGameObjects;

			if (DataTypeSent::CREATEPLAYER == dataObj.GenericData.DataType)
			{
				GameObject *go = new GameObject("PlayerNetworked");
				go->SetID(dataObj.GenericData.ObjectID);

				MainSingleton::Get().myNetworkQueue.emplace([go, dataObj, &gameobjects]()
															{

					go->transform->SetScale(CU::Vector4f(0.1f, 0.1f, 0.1f, 1));
					go->transform->SetPosition(CU::Vector4f(0.f, -0.2f, 0.f, 1));

					Material mat("Assets/Shaders/DefaultSpriteFS.glsl", "Assets/Shaders/DefaultSpriteVS.glsl");
					auto spriteRenderer = go->AddComponent<Sprite2DRenderer>(mat, 0);

					spriteRenderer->GetMaterial().SetTexture("Assets/Sprites/Person.png", DIFFUSE, SamplingType::Point);

					auto netTransform = go->AddComponent<NetworkTransform>();
					netTransform->sendData = false;

					//go->AddComponent<non::BoxCollider2D>(CU::Vector2f{0.05f, 0.05f});

					go->StartMyComponents();
					gameobjects.emplace(dataObj.GenericData.ObjectID, go); });
			}

			// if (dataObj.GenericData.ObjectID != 0)
			// 	if (gameobjects.find(dataObj.GenericData.ObjectID) == gameobjects.end())
			// 		CreateNetworkObject(dataObj, gameobjects);

			switch (dataObj.GenericData.DataType)
			{
			case DataTypeSent::JOINLOBBY:
			{
				int lobby;
				std::memcpy(&lobby, dataObj.Data.data(), sizeof(lobby));
				myLobbyPartOff = lobby;
			}
			break;
			case DataTypeSent::CONNECTIONINFOCLIENT:
			{
				// 0: Ping
				// 1: Packet loss
				float information[2];
				std::memcpy(&information, dataObj.Data.data(), sizeof(information));

				float ping = information[0];
				float pingMS = ping * 1000.f;

				float packetloss = information[1];
				float packetlossPercentage = packetloss * 100.f;

				myNetRole->myPacketloss = static_cast<int>(packetlossPercentage);
				myNetRole->myPing = pingMS;
			}
			break;
			case DataTypeSent::ASKAVAIBLELOBBIES:
			{
				int totalLobbies;
				std::memcpy(&totalLobbies, dataObj.Data.data(), sizeof(totalLobbies));

				myLobbiesAvaible.resize(totalLobbies);
				std::memcpy(myLobbiesAvaible.data(), dataObj.Data.data() + sizeof(totalLobbies), sizeof(totalLobbies) * totalLobbies);

				if (myLobbyPartOff != -1)
				{
					bool curretLobbyExist = false;
					for (auto &lobby : myLobbiesAvaible)
					{
						if (lobby == myLobbyPartOff)
						{
							curretLobbyExist = true;
							break;
						}
					}

					if (curretLobbyExist)
						myNetRole->SendData(&myLobbyPartOff, sizeof(myLobbyPartOff), DataTypeSent::JOINLOBBY, 0, 0, true);
					else
						myLobbyPartOff = -1;
				}
			}
			break;
			case DataTypeSent::CHANGETEXTURE:
			{
				if (gameobjects.find(goID) != gameobjects.end())
				{
					MainSingleton::Get().myNetworkQueue.emplace([dataObj, goID, &gameobjects]()
																{
						std::string texturePath(dataObj.Data.data(), dataObj.Data.data() + dataObj.GenericData.DataSize);
						gameobjects.at(goID)->GetComponent<Sprite2DRenderer>()->GetMaterial().SetTexture(texturePath.c_str(), DIFFUSE); });
				}
			}
			break;
			case DataTypeSent::CONNECT:
			{
				client->IsConnected(true);
				client->SendData(&goID, 1, DataTypeSent::ASKAVAIBLELOBBIES, 0, 0, true);
			}
			break;
			case DataTypeSent::KICK:
			{
				client->IsConnected(false);
			}
			break;
			case DataTypeSent::CREATEGAMEOBJECT:
			{
				if (gameobjects.find(dataObj.GenericData.ObjectID) == gameobjects.end())
					CreateNetworkObject(dataObj, gameobjects);
			}
			break;
			case DataTypeSent::CULLOBJECT:
			{
				if (gameobjects.find(goID) != gameobjects.end())
				{
					MainSingleton::Get().myNetworkQueue.emplace([dataObj, goID, &gameobjects]()
																{ gameobjects.at(goID)->GetComponent<Sprite2DRenderer>()->isVisible = false; });
				}
			}
			break;
			case DataTypeSent::DESTROYGAMEOBJECT:
			{
				for (int i = 0; i < MainSingleton::Get().myGameObjects.size(); i++)
				{
					auto &gameObject = MainSingleton::Get().myGameObjects[i];

					if (gameObject->GetRawID() == dataObj.GenericData.ObjectID)
						gameObject->Destroy();
				}
			}
			break;
			}

			for (const auto &[_, object] : MainSingleton::Get().myGameObjects)
			{
				if (!object)
					break;

				if (object->GetRawID() == dataObj.GenericData.ObjectID)
				{
					switch (dataObj.GenericData.DataType)
					{
					case DataTypeSent::TRANSFORM:
					{
						if (auto tmObject = object->GetComponent<NetworkTransform>())
						{
							if (gameobjects.find(goID) != gameobjects.end())
							{
								MainSingleton::Get().myNetworkQueue.emplace([dataObj, goID, &gameobjects]()
																			{ gameobjects.at(goID)->GetComponent<Sprite2DRenderer>()->isVisible = true; });
							}

							tmObject->RecieveDataNetwork(dataObj);
						}
					}
					break;
					}
				}
			}
		}
	}
}

NetRole *NetworkManager::GetNetRole()
{
	return myNetRole;
}

#endif