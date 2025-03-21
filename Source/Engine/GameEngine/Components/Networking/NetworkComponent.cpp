#include "../Engine/Engine.pch.h"
#include "NetworkComponent.h"

#include "../../NetworkManager.h"
#include "../Networking/Client/Client.h"
#include "../GameObject.h"

void NetworkComponent::Update(float /*aDeltaTime*/)
{
	MainSingleton::Get()->myNetworkManager->GetClient()->SendData(0, 0, DataTypeSent::DELTA, gameObject->GetRawID());
}
