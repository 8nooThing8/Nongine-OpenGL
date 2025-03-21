#pragma once

#include "../../Components/Component.h"

#include "../Networking/Shared/Common.h"

class CharacterController;

class NetworkComponent : public Component
{
public:
	NetworkComponent() = default;
	~NetworkComponent() = default;

	void Update(float aDeltaTime);

	virtual void RecieveDataNetwork(NetworkRecieveData& /*aNetworkDataRecieve*/)
	{

	}

	bool sendData = true;
	bool recieveData = true;

	DataTypeSent myDataType = DataTypeSent::NONE;
};