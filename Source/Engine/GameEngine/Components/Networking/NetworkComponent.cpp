#ifdef USENET

#include "NetworkComponent.h"

#include "../../NetworkManager.h"
#include "../Networking/Client/Client.h"
#include "../GameObject.h"

void NetworkComponent::Update(float /*aDeltaTime*/)
{
	//MainSingleton::Get().myNetworkManager->GetNetRole()->SendData(0, 0, DataTypeSent::DELTA, MainSingleton::Get().myNetworkManager->myLobbyPartOff, gameObject->GetRawID());
}

#endif