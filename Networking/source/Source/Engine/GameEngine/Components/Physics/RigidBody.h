#pragma once

#include "../Component.h"

namespace physx
{
	class PxRigidActor;
}

using namespace physx;

class RigidBody : public Component
{
public:
	RigidBody() = default;
	RigidBody(bool isStatic);
	~RigidBody() override;

	void SetDensity(float aDensity) const;

	void Start() override;
	void Update(float aDeltaTime) override;

	//void DrawBoxCollider();
	//void DrawSphereCollider();

	// Sets the RigidBody to static so it wont move or be affected by velocity in any way
	void SetIsStatic(bool aIsStatic);

	// Sets the linear velocity of the RigidBody also called the speed of the object
	void SetVelocity(const CU::Vector3<float>& aVelocity);
	// Gets the linear velocity of the RigidBody also called the speed of the object
	CU::Vector3<float> GetVelocity() const;
	float GetMagnitude();

	// Sets the angular velocity of the RigidBody also called the rotation speed of the object
	void SetAngularVelocity(const CU::Vector3<float>& aVelocity);
	// Gets the angular velocity of the RigidBody also called the rotation speed of the object
	const CU::Vector3<float> GetAngularVelocity();

	void Sleep() const;
	void Wake() const;

	//void OnCollision(GameObject*& aOther) override;
	//void OnCollisionEnter(GameObject*& aOther) override;
	//void OnCollisionExit(GameObject*& aOther) override;

	//void AddCollideWith(const CollisionLayer& aLayer);
	//void AddmyCollisionLayer(const CollisionLayer& aLayer);

	CommonUtilities::Vector3<float> mySize;

	PxRigidActor* GetActor() const;
private:
	bool myIsStatic;

	PxRigidActor* myBody;
	void UpdateTransform() const;

	//void SetMaterials(physx::PxMaterial* aMaterial) const;

	void UpdateStatic();
	void UpdateDynamic();
};