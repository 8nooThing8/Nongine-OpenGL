#ifdef USENET

#include "NetworkTransform.h"

#include "../../Components/GameObject.h"
#include "../../Components/Transform.h"

#include "../Engine/GameEngine/NetworkManager.h"
#include "../Networking/Client/Client.h"

#include <Timer.h>

#include "Engine/GameEngine/MainSingleton.h"

#include "Networking/Server/Server.h"

#include "GraphicsEngine.h"

#undef max

NetworkTransform::NetworkTransform()
{
	NetworkComponent();
	Component();

	myDataType = DataTypeSent::TRANSFORM;
}

void NetworkTransform::CullNetworkObjects(unsigned port, const ClientStorage &aClient, const NetworkTransformData &netTransform)
{
	CU::Vector2f position2F;
	std::memcpy(&position2F, &gameObject->transform->myPosition, sizeof(CU::Vector2f));

	CU::Vector2f positionDifference = aClient.clientPlayerPosition - position2F;

	if (positionDifference.Length() <= Server::myAwarnessCircle)
		MainSingleton::Get().myNetworkManager->GetNetRole()->SendData(&netTransform, sizeof(NetworkTransformData), myDataType, gameObject->myPartOfLobby, gameObject->GetRawID(), false, port);
	else
		MainSingleton::Get().myNetworkManager->GetNetRole()->SendData(&netTransform, 1, DataTypeSent::CULLOBJECT, gameObject->myPartOfLobby, gameObject->GetRawID(), false, port);
}

void NetworkTransform::ServerUpdateTransform()
{
	auto server = static_cast<Server *>(MainSingleton::Get().myNetworkManager->GetNetRole());
	auto &portToEndpoint = static_cast<Server *>(MainSingleton::Get().myNetworkManager->GetNetRole())->GetPortToSockAddr();

	const CU::Vector4f &position = gameObject->transform->GetPosition();
	const CU::Vector4f &rotation = gameObject->transform->GetRotation();
	const CU::Vector4f &scale = gameObject->transform->GetScale();

	bool transformIsDifferent = myLastPosition != position || myLastRotation != rotation || myLastScale != scale;

	int totalCommands = 0;

	auto &lobby = server->myLobbyToPort[gameObject->myPartOfLobby];

	for (auto &port : lobby)
	{
		auto &endpoint = portToEndpoint.at(port);

		CU::Vector2f position2F;
		std::memcpy(&position2F, &gameObject->transform->myPosition, sizeof(CU::Vector2f));
		CU::Vector2f positionDifference = endpoint.clientPlayerPosition - position2F;
		float differenceLength = positionDifference.Length();
		mySecondsPerUpdate = differenceLength / static_cast<float>(myUpdatesPerSecond);

		mySecondsPerUpdate = std::max(0.1f, mySecondsPerUpdate);

		if (myClientPortToLastTime[port].second >= mySecondsPerUpdate)
		{
			myClientPortToLastTime[port].first = CommonUtilities::Timer::GetTotalTime();

			if (transformIsDifferent)
			{
				totalCommands++;
				const size_t v3S = sizeof(CommonUtilities::Vector3<float>);

				NetworkTransformData netTransform;

				std::memcpy(&netTransform.Position, &gameObject->transform->myPosition, v3S);
				std::memcpy(&netTransform.Rotation, &gameObject->transform->myRotation, v3S);
				std::memcpy(&netTransform.Scale, &gameObject->transform->myScale, v3S);

				netTransform.SecondsPerUpdate = mySecondsPerUpdate;

				//MainSingleton::Get().myNetworkManager->GetNetRole()->SendData(&netTransform, sizeof(NetworkTransformData), myDataType, -1, gameObject->GetRawID(), false, port);
				CullNetworkObjects(port, endpoint, netTransform);
			}

			myLastPosition = gameObject->transform->myPosition;
			myLastRotation = gameObject->transform->myRotation;
			myLastScale = gameObject->transform->myScale;

			gameObject->transform->SetIsDirty();
		}
	}
}

void NetworkTransform::ClientUpdateTransform()
{
	if (myTimeProgression >= mySecondsPerUpdate)
	{
		myLastTime = CommonUtilities::Timer::GetTotalTime();

		const CU::Vector4f &position = gameObject->transform->GetPosition();
		const CU::Vector4f &rotation = gameObject->transform->GetRotation();
		const CU::Vector4f &scale = gameObject->transform->GetScale();

		if (myLastPosition != position || myLastRotation != rotation || myLastScale != scale)
		{
			const size_t v3S = sizeof(CommonUtilities::Vector3<float>);

			NetworkTransformData netTransform;

			std::memcpy(&netTransform.Position, &gameObject->transform->myPosition, v3S);
			std::memcpy(&netTransform.Rotation, &gameObject->transform->myRotation, v3S);
			std::memcpy(&netTransform.Scale, &gameObject->transform->myScale, v3S);

			netTransform.SecondsPerUpdate = mySecondsPerUpdate;

			auto &portToEndpoint = static_cast<Server *>(MainSingleton::Get().myNetworkManager->GetNetRole())->GetPortToSockAddr();

			MainSingleton::Get().myNetworkManager->GetNetRole()->SendData(&netTransform, sizeof(NetworkTransformData), myDataType, MainSingleton::Get().myNetworkManager->myLobbyPartOff, gameObject->GetRawID());
		}

		myLastPosition = gameObject->transform->myPosition;
		myLastRotation = gameObject->transform->myRotation;
		myLastScale = gameObject->transform->myScale;

		gameObject->transform->SetIsDirty();
	}
}

void NetworkTransform::Start()
{
	myUpdatesPerSecond = 10;

	mySecondsPerUpdate = 1.f / static_cast<float>(myUpdatesPerSecond);

	myFinalPosition = gameObject->transform->myPosition;
	myFinalRotation = gameObject->transform->myRotation;
	myFinalScale = gameObject->transform->myScale;

	// ForceUpdateTransform();
}

void NetworkTransform::Update(float aDeltaTime)
{
	float currentFPS = GraphicsEngine::Get().GetFPS();
#ifdef ISSERVER
	auto server = static_cast<Server *>(MainSingleton::Get().myNetworkManager->GetNetRole());
	auto &portToEndpoint = server->GetPortToSockAddr();

	auto &lobby = server->myLobbyToPort[gameObject->myPartOfLobby];
	for (auto &port : lobby)
	{
		auto &endpoint = portToEndpoint.at(port);

		myClientPortToLastTime[port].second = CommonUtilities::Timer::GetTotalTime() - myClientPortToLastTime[port].first;
	}
#else
	myTimeProgression = CommonUtilities::Timer::GetTotalTime() - myLastTime;
#endif

	if (shouldLerp)
	{
		float lerpTime = (CommonUtilities::Timer::GetTotalTime() - myLerpStartTime) / mySecondsPerUpdate;
		lerpTime = std::min<float>(1.f, lerpTime);

		CU::Vector4f lerpedPosition = CU::Lerp<CU::Vector4f>(myLastPosition, myFinalPosition, lerpTime);
		CU::Vector4f lerpedRotation = CU::Lerp<CU::Vector4f>(myLastRotation, myFinalRotation, lerpTime);
		CU::Vector4f lerpedScale = CU::Lerp<CU::Vector4f>(myLastScale, myFinalScale, lerpTime);

		auto &transform = gameObject->transform;

		transform->myPosition = lerpedPosition;
		transform->myRotation = lerpedRotation;
		transform->myScale = lerpedScale;

		gameObject->transform->SetIsDirty();

		if (lerpTime >= 1)
		{
			shouldLerp = false;
		}
	}

	if (!MainSingleton::Get().myNetworkManager->GetNetRole() || !sendData)
	{
		if (myTimeProgression >= mySecondsPerUpdate)
			myLastTime = CommonUtilities::Timer::GetTotalTime();

		return;
	}

#ifdef ISSERVER
	ServerUpdateTransform();
#else
	ClientUpdateTransform();
#endif
}

void NetworkTransform::ForceUpdateTransform()
{
	if (!sendData)
		return;

	const size_t v3S = sizeof(CommonUtilities::Vector3<float>);

	NetworkTransformData netTransform;

	std::memcpy(&netTransform.Position, &gameObject->transform->myPosition, v3S);
	std::memcpy(&netTransform.Rotation, &gameObject->transform->myRotation, v3S);
	std::memcpy(&netTransform.Scale, &gameObject->transform->myScale, v3S);

	netTransform.SecondsPerUpdate = mySecondsPerUpdate;
#ifdef ISSERVER
	MainSingleton::Get().myNetworkManager->GetNetRole()->SendData(&netTransform, sizeof(NetworkTransformData), myDataType, gameObject->myPartOfLobby, gameObject->GetRawID());
#else
	MainSingleton::Get().myNetworkManager->GetNetRole()->SendData(&netTransform, sizeof(NetworkTransformData), myDataType, MainSingleton::Get().myNetworkManager->myLobbyPartOff, gameObject->GetRawID());
#endif
}

void NetworkTransform::RecieveDataNetwork(const NetworkData::PerObjectData &aNetworkDataRecieve)
{
	if (!MainSingleton::Get().myNetworkManager->GetNetRole() || !recieveData)
		return;

	const size_t v3S = sizeof(CommonUtilities::Vector3<float>);

	const char *dataRecieve = aNetworkDataRecieve.Data.data();

	myLastPosition = gameObject->transform->myPosition;
	myLastRotation = gameObject->transform->myRotation;
	myLastScale = gameObject->transform->myScale;

	NetworkTransformData netTransform;

	memcpy(&netTransform, dataRecieve, sizeof(NetworkTransformData));

	memcpy(&myFinalPosition, &netTransform.Position, v3S);
	memcpy(&myFinalRotation, &netTransform.Rotation, v3S);
	memcpy(&myFinalScale, &netTransform.Scale, v3S);

	mySecondsPerUpdate = netTransform.SecondsPerUpdate;

	if (firstRecieve)
	{
		gameObject->transform->myPosition = myFinalPosition;
		gameObject->transform->myRotation = myFinalRotation;
		gameObject->transform->myScale = myFinalScale;

		firstRecieve = false;
		return;
	}

	shouldLerp = true;
	myLerpStartTime = CommonUtilities::Timer::GetTotalTime();
}
#endif