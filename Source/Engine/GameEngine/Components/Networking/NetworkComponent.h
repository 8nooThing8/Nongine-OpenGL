#pragma once
#ifdef USENET
#include "../../Components/Component.h"

#include "../Networking/Shared/NetRole.h"

class NetworkComponent : public Component
{
public:
	NetworkComponent() = default;
	~NetworkComponent() = default;

	void Update(float aDeltaTime);

	virtual void RecieveDataNetwork(RecieveDataReturn & /*aNetworkDataRecieve*/)
	{
	}

	bool sendData = true;
	bool recieveData = true;

	int myUpdatesPerSecond = 20;

	DataTypeSent myDataType = DataTypeSent::ERR;

protected:
	float myLastTime = 0;
	float myTimeProgression = 0;
	bool shouldLerp = false;
	float myLerpStartTime = 0;
	float mySecondsPerUpdate = 0;

};

#endif