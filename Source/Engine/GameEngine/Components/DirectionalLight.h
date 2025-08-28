#pragma once

#include "../Engine/GameEngine/Components/Component.h"
#include "../Engine/GameEngine/GameCamera.h"
#include <Vector/Vector3.hpp>

class DirectionalLight : public Component
{
public:
	DirectionalLight();
	~DirectionalLight() = default;

	void BindLightFrameBuffer();

	void Start() override;
	void EarlyUpdate(float) override;

	void SetColor(const CU::Vector3f& aColor);
	void SetIntensity(float aIntensity);

	CommonUtilities::GameCamera GetCamera() { return myCamera; };

	static inline void SetInstance(DirectionalLight* aLight)
	{
		instance = aLight;
	}
	static inline DirectionalLight* Get()
	{
		return instance;
	}
	inline unsigned int GetShadowMapTexture()
	{
		return myShadowMapTexture;
	}

private:
	float myIntensity;
	CommonUtilities::GameCamera myCamera;
	CommonUtilities::Vector3<float> myColor;
	const float myShadowTextureSize = 2048;

	unsigned int myShadowMapTexture;
	unsigned int myShadowMapFrameBuffer;

	static inline DirectionalLight* instance = nullptr;
};