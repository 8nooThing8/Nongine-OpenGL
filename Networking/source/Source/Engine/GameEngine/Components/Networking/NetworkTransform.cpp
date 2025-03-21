#include "../Engine/Engine.pch.h"

#include "NetworkTransform.h"

#include "../../Components/GameObject.h"
#include "../../Components/Transform.h"

#include "../Engine/GameEngine/NetworkManager.h"

#include <InputHandler.h>

#include "../../GameCamera.h"

#include "../Engine/GameEngine/Components/Physics/CharacterController.h"

#include "../Networking/Client/Client.h"

NetworkTransform::NetworkTransform()
{
	Component();

	vector3Size = sizeof(CommonUtilities::Vector3<float>);
}

void NetworkTransform::Start()
{
	MainSingleton::Get()->myNetworkManager->SubscribeMeToNetworkRecieve(this, DataTypeSent::TRANSFORM);

	myCC = gameObject->GetComponent<CharacterController>();

	position = &gameObject->transform->position;
	rotation = &gameObject->transform->rotation;
	scale = &gameObject->transform->scale;
}

void NetworkTransform::Update(float /*aDeltaTime*/)
{
	if (MainSingleton::Get()->myNetworkManager->GetClient())
	{
		if (sendData)
		{
			int fwAmount = InputHandler::GetInstance()->GetKey(keycode::W) - InputHandler::GetInstance()->GetKey(keycode::S);
			int lrAmount = InputHandler::GetInstance()->GetKey(keycode::D) - InputHandler::GetInstance()->GetKey(keycode::A);

			CommonUtilities::Vector4<float> forward = GameCamera::main->myForward;
			CommonUtilities::Vector4<float> right = GameCamera::main->myRight;

			forward.y = 0.0f;
			right.y = 0.0f;

			forward.Normalize();
			right.Normalize();

			auto vel = CommonUtilities::Vector4<float>(static_cast<float>(fwAmount) * forward + static_cast<float>(lrAmount) * right);

			CommonUtilities::Vector2f moveVec(vel.x, vel.z);

			moveVec.Normalize();

			myCC->SetVelocity(CommonUtilities::Vector3f(moveVec.x * 500.f, myCC->GetVelocity().y, moveVec.y * 500.f));

			if (myCC->IsStandingOnGround())
			{
				myCC->GetVelocity().y = 0;

				if (InputHandler::GetInstance()->GetKey(keycode::SPACE))
				{
					myCC->AddVelocity(CommonUtilities::Vector3f(0, 1000, 0));
				}
			}

			gameObject->transform->rotation.y = GameCamera::main->rotation.y * deg2rad;

			if (lastPosition != *position || lastRotation != *rotation || lastScale != *scale)
			{
				char dataToSend[DEFAULT_BUFLEN] = {};

				memcpy(dataToSend, &position->x, vector3Size);
				memcpy(dataToSend + vector3Size, &rotation->x, vector3Size);
				memcpy(dataToSend + vector3Size * 2, &scale->x, vector3Size);

				MainSingleton::Get()->myNetworkManager->GetClient()->SendData(dataToSend, sizeof(Vector3f) * 3, DataTypeSent::TRANSFORM, gameObject->myID);
			}

			lastPosition = *position;
			lastRotation = *rotation;
			lastScale = *scale;
		}
	}
}

void NetworkTransform::RecieveDataNetwork(NetworkRecieveData& aNetworkDataRecieve)
{
	if (MainSingleton::Get()->myNetworkManager->GetClient())
	{
		if (!sendData)
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
}
