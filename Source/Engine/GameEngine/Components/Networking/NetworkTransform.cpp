#include "../Engine/Engine.pch.h"

#include "NetworkTransform.h"

#include "../../Components/GameObject.h"
#include "../../Components/Transform.h"

#include "../Engine/GameEngine/NetworkManager.h"

#include "../Networking/Client/Client.h"

NetworkTransform::NetworkTransform()
{
	NetworkComponent();
	Component();

	myDataType = DataTypeSent::TRANSFORM;

	vector3Size = sizeof(CommonUtilities::Vector3<float>);
}

void NetworkTransform::Start()
{
	//MainSingleton::Get()->myNetworkManager->SubscribeMeToNetworkRecieve(this, DataTypeSent::TRANSFORM);

	position = &gameObject->transform->position;
	rotation = &gameObject->transform->rotation;
	scale = &gameObject->transform->scale;
}

void NetworkTransform::UpdateEditor(float /*aDeltaTime*/)
{
	if (MainSingleton::Get()->myNetworkManager->GetClient())
	{
		if (lastPosition != *position || lastRotation != *rotation || lastScale != *scale)
		{
			char dataToSend[DEFAULT_BUFLEN] = {};

			memcpy(dataToSend, &position->x, vector3Size);
			memcpy(dataToSend + vector3Size, &rotation->x, vector3Size);
			memcpy(dataToSend + vector3Size * 2, &scale->x, vector3Size);

			MainSingleton::Get()->myNetworkManager->GetClient()->SendData(dataToSend, sizeof(Vector3f) * 3, myDataType, gameObject->GetRawID());
		}

		lastPosition = *position;
		lastRotation = *rotation;
		lastScale = *scale;
	}
}

void NetworkTransform::Update(float aDeltaTime)
{
	NetworkComponent::Update(aDeltaTime);
}

void NetworkTransform::ForceUpdateTransform()
{
	char dataToSend[DEFAULT_BUFLEN] = {};

	memcpy(dataToSend, &position->x, vector3Size);
	memcpy(dataToSend + vector3Size, &rotation->x, vector3Size);
	memcpy(dataToSend + vector3Size * 2, &scale->x, vector3Size);

	MainSingleton::Get()->myNetworkManager->GetClient()->SendData(dataToSend, sizeof(Vector3f) * 3, myDataType, gameObject->GetRawID());
}


void NetworkTransform::RecieveDataNetwork(NetworkRecieveData& aNetworkDataRecieve)
{
	if (MainSingleton::Get()->myNetworkManager->GetClient())
	{
		if (aNetworkDataRecieve.bytesSent > 0)
		{
			const char* dataRecieve = aNetworkDataRecieve.data;

			CommonUtilities::Vector3<float> pos;
			CommonUtilities::Vector3<float> rot;
			CommonUtilities::Vector3<float> sca;

			memcpy(&pos.x, dataRecieve, vector3Size);
			memcpy(&rot.x, dataRecieve + vector3Size, vector3Size);
			memcpy(&sca.x, dataRecieve + vector3Size * 2, vector3Size);

			memcpy(&position->x, &pos.x, vector3Size);
			memcpy(&rotation->x, &rot.x, vector3Size);
			memcpy(&scale->x, &sca.x, vector3Size);
		}

	}
}

void NetworkTransform::RenderImGUI()
{
	ImGui::Checkbox("SendData", &sendData);
	ImGui::Checkbox("RecieveData", &recieveData);
}
