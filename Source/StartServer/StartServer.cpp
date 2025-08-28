#ifndef OPENGL
#define OPENGL
#endif // OPENGL

#include <iostream>

#include "Server/Server.h"

#include <thread>

#include "Engine/GameEngine/NetworkManager.h"
#include "Engine/GameEngine/MainSingleton.h"
#include "Engine/GameEngine/Components/Game/MoveServerObject.h"
#include "Engine/GameEngine/Components/GameObject.h"
#include "Engine/GameEngine/Components/Networking/NetworkTransform.h"

#include "Timer.h"
#include "Application/editor.h"

#include <random>

#include <fstream>
#include <filesystem>

namespace CU = CommonUtilities;

bool hasStarted = false;

void RecieveDataFuncThread()
{
	hasStarted = true;
	NetworkManager *networkManager = MainSingleton::Get().myNetworkManager;
	networkManager->StartGameServer();
	Server *server = static_cast<Server *>(networkManager->GetNetRole());

	while (true)
	{
	}
}

bool removePort;
std::vector<unsigned> removePorts;

void CheckGarantiedMessage()
{
	NetworkManager *networkManager = MainSingleton::Get().myNetworkManager;
	Server *server = static_cast<Server *>(networkManager->GetNetRole());

	if (!server)
		return;

	float totalTime = CU::Timer::GetTotalTime();

	auto &clientgarantiedMessaged = server->GetGarantiedMessages();

	server->DeleteMessages();

	for (auto &client : clientgarantiedMessaged)
	{
		if (totalTime >= client.second.timeAtSend + 0.5f)
		{
			NetroleStorage &aClientStorage = client.second;

			if (aClientStorage.currentTimeoutCount >= aClientStorage.timesUntilTimeout)
			{
				removePort = true;
				removePorts.emplace_back(aClientStorage.port);
			}
			else if (aClientStorage.currentTimeoutCount)
			{
				auto &recentPacketLoss = server->GetPortToSockAddr().at(aClientStorage.port).recentPacketLoss;
				recentPacketLoss++;
			}

			std::cout << "Try again" << std::endl;

			reinterpret_cast<Server *>(MainSingleton::Get().myNetworkManager->GetNetRole())->SendDataDirectly(aClientStorage.dataSent, aClientStorage.dataSize, aClientStorage.port);
			aClientStorage.currentTimeoutCount++;
			aClientStorage.timeAtSend = CU::Timer::GetTotalTime();
		}
	}
}

void SendFileServ()
{
	std::string input;

	std::getline(std::cin, input);

	std::filesystem::path fileToSend(input);
	int fileSize = std::filesystem::file_size(fileToSend);

	std::ifstream fileStr(fileToSend.c_str(), std::ios_base::binary);
	char *messageToSend = static_cast<char *>(std::malloc(fileSize));

	int characterOffset = 0;

	while (fileStr.get(messageToSend[characterOffset]))
		characterOffset++;

	fileStr.close();

	Server *server = reinterpret_cast<Server *>(MainSingleton::Get().myNetworkManager->GetNetRole());

	server->SendFile(messageToSend, fileSize, fileToSend.filename().string().c_str());

	delete messageToSend;
}

#define COOLMODE 0
int main()
{
	// GameObject::nextID = GameObject::intDiv2;

	srand(time(NULL));

	std::thread thread = std::thread(RecieveDataFuncThread);

	float lastTotalTime = 0;

	int networkGOCount = 0;

	float creationTimer = 0.f;
	float pingCheck = 0.05f;

	GameObject *go;
	Editor::Get().LoadSceneNetwork("scene", false);

	Server *server = nullptr;
	do
	{
		server = reinterpret_cast<Server *>(MainSingleton::Get().myNetworkManager->GetNetRole());
	} while (!server);

	server->myLobbyToPort[0];
	server->myLobbyToPort[1];

	// SendFileServ();

	while (true)
	{
		if (server->nextTimeToUpdateSentRec <= 0.f)
		{
			server->nextTimeToUpdateSentRec = 1.f;

			server->mySentPerSecond = server->myTotalDataSent;
			server->myRecievePerSecond = server->myTotalDataRecieved;

			server->myTotalDataRecieved = 0;
			server->myTotalDataSent = 0;

			for (auto &port : server->GetPortToSockAddr())
			{
				int recentPackageLoss = port.second.recentPacketLoss;
				int totalPackages = port.second.totalPacketsSent;

				float recentPackageLossFloat = static_cast<float>(recentPackageLoss);
				float totalPackagesFloat = static_cast<float>(totalPackages);

				float packageLoss = recentPackageLossFloat / totalPackagesFloat;
				float packageLossPercentage = packageLoss * 100;

				std::cout << "Port " << port.first << ", ";
				std::cout << "ping:" << port.second.ping * 1000.f << "ms";
				std::cout << "    Packet loss: " << packageLossPercentage << "%" << std::endl;

				int sentPS = server->GetSentPerSecond();
				std::cout << "Sent: " << sentPS << "/s,      ";

				int recievedPS = server->GetRecievedPerSecond();
				std::cout << "Recieved: " << recievedPS << "/s" << std::endl;

				std::cout << std::endl;
			}
		}
		else
		{
			server->nextTimeToUpdateSentRec -= CommonUtilities::Timer().GetDeltaTime();
		}

		server->KickClients();

		const float totalTime = CU::Timer::GetTotalTime();

		CU::Timer::Update();

		if (pingCheck <= 0)
		{
			server->SendData(&pingCheck, 1, DataTypeSent::PINGCHECKSERVER, -1, 0, true);

			pingCheck = 0.5f;
		}
		else
		{
			pingCheck -= CU::Timer::GetDeltaTime();
		}

		// Lobby 0
		if (creationTimer <= 0)
		{
			creationTimer = 0.25f;
			GameObject *go = new GameObject("NetworkObject");
			MainSingleton::Get().myGameObjects.emplace(go->GetRawID(), go);

			auto moveServerObject = go->AddComponent<MoveServerObject>();
			auto networkTransform = go->AddComponent<NetworkTransform>();
			go->myPartOfLobby = 0;

			moveServerObject->myVelocity = {0.7f, 0.f};

			go->transform->SetScale(CU::Vector4f{0.1f, 0.1f, 0.1f, 1.f});
			go->transform->SetPosition(CU::Vector4f{1.f, 0.5f, 0.f, 1.f});
			go->StartMyComponents();
		}
		else if (COOLMODE)
		{
			creationTimer -= CU::Timer::GetDeltaTime();
		}

		// Lobby 1
		if (creationTimer <= 0)
		{
			creationTimer = 0.25f;
			GameObject *go = new GameObject("NetworkObject");
			MainSingleton::Get().myGameObjects.emplace(go->GetRawID(), go);

			auto moveServerObject = go->AddComponent<MoveServerObject>();
			auto networkTransform = go->AddComponent<NetworkTransform>();
			go->myPartOfLobby = 1;

			moveServerObject->myVelocity = {0.7f, 0.5f};

			go->transform->SetScale(CU::Vector4f{0.1f, 0.1f, 0.1f, 1.f});
			go->transform->SetPosition(CU::Vector4f{1.f, 0.5f, 0.f, 1.f});
			go->StartMyComponents();
		}

		if (removePort)
		{
			for (const auto &port : removePorts)
				server->KickClient(port);

			removePort = false;
			removePorts.clear();
		}

		MainSingleton::Get().myComponentManager->EarlyUpdateComponents(CU::Timer::GetDeltaTime());
		MainSingleton::Get().myComponentManager->UpdateComponents(CU::Timer::GetDeltaTime());

		server->CommitDataSend();

		CheckGarantiedMessage();
	}

	thread.join();
}
