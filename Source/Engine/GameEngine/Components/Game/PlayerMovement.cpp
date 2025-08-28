#include "PlayerMovement.h"
#include <InputHandler.h>

#include "../GameObject.h"

#include "Vector/Vector.hpp"

#include "GLFW/glfw3.h"

#ifdef USENET
#include "Engine/GameEngine/NetworkManager.h"
#include "Networking/Shared/NetRole.h"
#endif

#include <Timer.h>

PlayerMovement::PlayerMovement()
{
}

void PlayerMovement::Start()
{
	cayoteeTimer = 0;
	myMoveSpeed = 0.75f;
	//myMaxMoveSpeed = 0.0001f;
	//myMoveAccSpeed = 0.02f;

	rb = gameObject->GetComponent<non::RigidBody2D>();
}

void PlayerMovement::Update(float aDeltaTime)
{
	CU::Vector4f moveDir;

	bool WPress = glfwGetKey(MainSingleton::Get().myWindow, GLFW_KEY_W) == GLFW_PRESS;
	bool SPress = glfwGetKey(MainSingleton::Get().myWindow, GLFW_KEY_S) == GLFW_PRESS;
	bool DPress = glfwGetKey(MainSingleton::Get().myWindow, GLFW_KEY_D) == GLFW_PRESS;
	bool APress = glfwGetKey(MainSingleton::Get().myWindow, GLFW_KEY_A) == GLFW_PRESS;

	moveDir.x = static_cast<float>(DPress - APress);
	moveDir.y = static_cast<float>(WPress - SPress);

	moveDir.Normalize();

	rb->GetVelocity().x = moveDir.x * myMoveSpeed * aDeltaTime;
	rb->GetVelocity().y = moveDir.y * myMoveSpeed * aDeltaTime;
#ifdef USENET
	const CU::Vector4f& position = gameObject->transform->GetPosition();

	if (sendDataTimer <= 0)
	{
		sendDataTimer = 0.5f;
		MainSingleton::Get().myNetworkManager->GetNetRole()->SendData(&position, sizeof(CommonUtilities::Vector2f), DataTypeSent::MOVEPLAYEROBJECT, MainSingleton::Get().myNetworkManager->myLobbyPartOff, gameObject->GetRawID());
	}
	else
	{
		sendDataTimer -= CommonUtilities::Timer::GetDeltaTime();
	}
#endif


	// if (glfwGetKey(MainSingleton::Get().myWindow, GLFW_KEY_O) == GLFW_RELEASE)
	// {
	// 	std::string newSpriteName = "Assets/Sprites/Person.png";
	// 	MainSingleton::Get().myNetworkManager->GetNetRole()->SendData(newSpriteName.c_str(), newSpriteName.size(), DataTypeSent::CHANGETEXTURE, MainSingleton::Get().myNetworkManager->myLobbyPartOff, myID, true);
	// }
	

	// if ((InputHandler::GetKeyDown(keycode::SPACE) || InputHandler::GetKeyDown(keycode::W)) && cayoteeTimer <= cayoteeTime)
	// {
	// 	rb->GetVelocity().y = 1.15f;
	// }
}
