#pragma once

#include "../../Components/Component.h"

class CharacterController;

namespace CommonUtilities
{
	template <class T>
	class Vector4;
}

#include "../Networking/Shared/NetRole.h"

class NetworkTransform : public Component
{
public:
	NetworkTransform();
	~NetworkTransform() = default;

	void Start() override;
	void Update(float aDeltaTime) override;

	void RecieveDataNetwork(NetworkRecieveData& aNetworkDataRecieve);

	bool sendData;
private:
	CharacterController* myCC;

	size_t vector3Size;

	CommonUtilities::Vector4<float>* position;
	CommonUtilities::Vector4<float>* rotation;
	CommonUtilities::Vector4<float>* scale;

	CommonUtilities::Vector4<float> lastPosition;
	CommonUtilities::Vector4<float> lastRotation;
	CommonUtilities::Vector4<float> lastScale;
};