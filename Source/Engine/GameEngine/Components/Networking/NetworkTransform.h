#pragma once

#include "../Networking/NetworkComponent.h"

#include <Vector/Vector4.hpp>

#include "../Networking/Shared/NetRole.h"

class CharacterController;

class NetworkTransform : public NetworkComponent
{
public:
	NetworkTransform();
	~NetworkTransform() = default;

	void Start() override;
	void UpdateEditor(float aDeltaTime) override;
	void Update(float aDeltaTime) override;
	void ForceUpdateTransform();

	void RecieveDataNetwork(NetworkRecieveData& aNetworkDataRecieve);

	void RenderImGUI() override;
private:
	size_t vector3Size;

	CommonUtilities::Vector4<float>* position;
	CommonUtilities::Vector4<float>* rotation;
	CommonUtilities::Vector4<float>* scale;

	CommonUtilities::Vector4<float> lastPosition;
	CommonUtilities::Vector4<float> lastRotation;
	CommonUtilities::Vector4<float> lastScale;

	bool sendPosition;
	bool sendRotation;
	bool sendScale;
};