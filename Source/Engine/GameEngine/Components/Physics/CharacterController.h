#pragma once

#include <characterkinematic/PxController.h>
#include "../PhysicsEngine/PhysicsEngine.h"

#include "../Component.h"
#include "Vector\Vector.hpp"

namespace physx
{
	class PxController;
}

class ControllerCallback : public PxControllerFilterCallback
{
public:
	ControllerCallback() = default;
	~ControllerCallback() override = default;

	bool filter(const PxController& /*a*/, const PxController& /*b*/) override
	{
		return true;
	}
};



class CharacterController : public Component
{
public:
	CharacterController() = default;
	CharacterController(float aHalfHeight, float aRadius, CollisionLayer aCollisionLayer, const CommonUtilities::Vector3f& aRotOffset = CommonUtilities::Vector3f(0, 0, 0));
	CharacterController(float aHalfHeight, float aRadius);
	~CharacterController() override;

	//void Pause() override;
	//void UnPause() override;

	void Start() override;
	void Update(float aDeltaTime) override;

	void SetCollisionLayer(CollisionLayer aLayer);

	void SetVelocity(const CommonUtilities::Vector3f& aVelocity);
	void AddVelocity(const CommonUtilities::Vector3f& aVelocity);
	CommonUtilities::Vector3f& GetVelocity();

	void TurnOffGravity();
	void SetUseGravity(bool aUseGravity);

	void SetFootPosition(const CommonUtilities::Vector3f& aFootPosition);
	void SetPosition(const CommonUtilities::Vector3f& aFootPosition);
	void SetStepHeight(float aStepHeight) const;
	void SetGravityFactor(float aFactor);

	//Component* NewComponentOfSameType() override;


	physx::PxController*& GetController();

	bool& GetHasMesh() { return myHasMesh; }

	float GetRadius() const;
	bool IsStandingOnGround() const;

	void SetRadius(float aRadius);
	void SetHalfHeigt(float aHalfHeight);

	CommonUtilities::Vector3f offset = CommonUtilities::Vector3f(0, 0, 0);
protected:
	bool myHasMesh;

private:
	bool lastIsStandingOnGround;

	bool characterocntorllerOn = true;

	CommonUtilities::Vector3f myVelocity = CommonUtilities::Vector3f();
	physx::PxController* myController;

	CollisionLayer myCollisionLayer;

	physx::PxFilterData myFilterData;
	physx::PxControllerFilters myFilters;
	bool myDrawColliders = false;
	bool myUseGravity = true;

	bool removedActor = false;

	PxControllerCollisionFlags myCurrentCollisionFlags;

	PxCapsuleControllerDesc myControllerProperties;

	//Temporary vaiable potensioally
	float myRadius = 40;
	float myHalfHeight = 80;
	CommonUtilities::Vector3f myGravity = CommonUtilities::Vector3f(0.f, -3000.f, 0.f);


	// temp
	CommonUtilities::Vector3f rotOffset;
};