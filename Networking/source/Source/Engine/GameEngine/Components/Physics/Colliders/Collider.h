#pragma once

#include "../../Component.h"
#include "Vector\Vector3.hpp"
#include "Quaternion/Quaternion.hpp"

#include "../PhysicsEngine/PhysicsEngine.h"

namespace physx
{
	class PxShape;
}

class RigidBody;
//struct PhysicsMaterial
//{
//	/**
//	Create a new physics material which can be applied to a collider to change how it acts and interacts with the world
//	*
//	*@param[in] aStaticFriction The friction that occurs between two bodies in contact with one another while they are at rest.
//	*@param[in] aDynamicFriction The friction that opposes the movement of a body which is already in motion.
//	*@param[in] aBounciness How elastic is the object higher gives more bounce.
//	*/
//	PhysicsMaterial();
//	PhysicsMaterial(float aStaticFriction, float aDynamicFriction, float aBounciness) : staticFriction(aStaticFriction), dynamicFriction(aDynamicFriction), bounciness(aBounciness) {}
//	PhysicsMaterial(PhysicsMaterial& physMat) = default;

//	float staticFriction;
//	float dynamicFriction;
//	float bounciness;
//};

class Collider : public Component
{
public:

	Collider() = default;

	Collider(PxShape* aShape);
	//Collider(PxShape* aShape, const PhysicsMaterial& aPhysicsMaterial);
	void SetCollisionLayer(CollisionLayer aCollisionLayer, bool aValue = true);
	//Collider(PxShape* aShape, float aStaticFriction, float aDynamicFriction, float aBounciness);
	~Collider() override;

	//void Start(PxShape* aShape);
	virtual void UpdateTransform() const;

	virtual void Update(float) override;
	//void Pause() override;
	//void UnPause() override;

	virtual void RenderImGUI() override;

	void Disable();
	void Enable();

	void Start() override;

	virtual void DrawCollider() {};

	// Do not use to set if it is a trigger, is only suppose to be used before start is ran
	bool& GetIsTrigger()
	{
		return myIsTrigger;
	}

public:
	void SetOffsets(const CommonUtilities::Vector3f& aPositionOffset, const CommonUtilities::Vector3f& aRotationOffset);
	void SetPositionOffset(const CommonUtilities::Vector3f& aPositionOffset);
	void SetRotationOffsetEuler(const CommonUtilities::Vector3f& aRotationOffset);
	void SetRotationOffsetQuaternion(const CommonUtilities::Quaternion& aRotationOffset);

public:
	void SetIsTrigger(bool isTrigger);
	//void SetPhysicsMaterial(const PhysicsMaterial& aPhysicsMaterial);
	//void SetPhysicsMaterial(float aStaticFriction, float aDynamicFriction, float aBounciness);

public:
	PxShape* GetShape() const;
	void SetShape(PxShape* aShape);

	bool myIsDisabled = false;

	bool removedActor = false;

protected:
	//PhysicsMaterial myPhysicsMaterial;

	CollisionLayer myCollisionLayer;

	PxShape* myShape;

	PxRigidActor* myActor = nullptr;

	CU::Vector3f myPositionOffset;
	CU::Quaternion myRotationOffset = CU::Quaternion(0, 0, 0, 1);

	bool myIsTrigger;

private:
	RigidBody* myRb;
};