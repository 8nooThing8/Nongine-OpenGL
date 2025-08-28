#pragma once

#ifdef USENET

#include "../Networking/NetworkComponent.h"

#include <Vector/Vector4.hpp>
#include <Vector/Vector3.hpp>

#include "../Networking/Shared/NetRole.h"

#include <unordered_map>
#include <utility>

struct ClientStorage;

struct NetworkTransformData
{
	CommonUtilities::Vector3<float> Position;
	CommonUtilities::Vector3<float> Rotation;
	CommonUtilities::Vector3<float> Scale;

	float SecondsPerUpdate;
};

class NetworkTransform : public NetworkComponent
{
public:
	NetworkTransform();
	~NetworkTransform() = default;

	void ServerUpdateTransform();
	void ClientUpdateTransform();

	void CullNetworkObjects(unsigned port, const ClientStorage& aClient, const NetworkTransformData& netTransform);

	void Start() override;
	void Update(float aDeltaTime) override;
	void ForceUpdateTransform();

	void RecieveDataNetwork(const NetworkData::PerObjectData& aNetworkDataRecieve);

	//void RenderImGUI() override;
private:
	CommonUtilities::Vector4<float> myFinalPosition;
	CommonUtilities::Vector4<float> myFinalRotation;
	CommonUtilities::Vector4<float> myFinalScale;

	CommonUtilities::Vector4<float> myLastPosition;
	CommonUtilities::Vector4<float> myLastRotation;
	CommonUtilities::Vector4<float> myLastScale;

	CommonUtilities::Vector4<float> newPosition;
	CommonUtilities::Vector4<float> newRotation;
	CommonUtilities::Vector4<float> newScale;

	bool sendPosition;
	bool sendRotation;
	bool sendScale;

	bool firstRecieve = true;

	std::unordered_map<unsigned, std::pair<float, float>> myClientPortToLastTime;
};

#endif