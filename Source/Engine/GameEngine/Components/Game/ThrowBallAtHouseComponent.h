#pragma once

#include "../Component.h"

#include "../Physics/RigidBody.h"

class ThrowBallAtHouseComponent : public Component
{
public:
	ThrowBallAtHouseComponent() = default;
	~ThrowBallAtHouseComponent() = default;

private:
	virtual void Start() override;
	virtual void Update(float /*aDeltaTime*/) override;

	float totalScroll;
};

