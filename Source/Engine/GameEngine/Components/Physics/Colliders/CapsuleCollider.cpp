#include "../Engine/Engine.pch.h"

#include "CapsuleCollider.h"


#include "../Engine/GameEngine/MainSingleton.h"
#include "../Engine/GameEngine/SceneManagerInternal.h"
#include "../Engine/GameEngine/Components/GameObject.h"

#include "Vector/Vector.hpp"

using namespace physx;

const std::array directionDecryptor
{
	CU::Vector3f(0, 0, 1.570795f),
	CU::Vector3f(0, 0, 0),
	CU::Vector3f(0, 1.570795f, 0)
};

CapsuleCollider::CapsuleCollider(float aRadius, float aHalfHeight, Direction aUpDirection) : Collider(PhysicsEngine::Get().CreateCapsuleCollider(aRadius, aHalfHeight)), myRadius(aRadius), myHalfHeight(aHalfHeight), myUpDirection(aUpDirection)
{
}

CapsuleCollider::CapsuleCollider() : Collider(PhysicsEngine::Get().CreateCapsuleCollider(1, 1)), myRadius(1), myHalfHeight(1), myUpDirection(Up)
{
	myCollisionLayer = CollisionLayer::Default;
}

CapsuleCollider::CapsuleCollider(float aRadius, float aHalfHeight, CollisionLayer aCollisionLayer, Direction aUpDirection) : Collider(PhysicsEngine::Get().CreateCapsuleCollider(aRadius, aHalfHeight)), myRadius(aRadius), myHalfHeight(aHalfHeight), myUpDirection(aUpDirection)
{
	SetCollisionLayer(aCollisionLayer);
}

void CapsuleCollider::UpdateTransform() const
{
	CommonUtilities::Vector3f position = gameObject->transform->position;
	CommonUtilities::Quaternion rotation = (directionDecryptor[myUpDirection]).ToQuarternion();

	PxTransform tmActor;
	tmActor.p = PxVec3(position);
	tmActor.q = PxQuat(rotation);

	myActor->setGlobalPose(tmActor);

	physx::PxTransform tmCollider;
	tmCollider.p = physx::PxVec3(myPositionOffset);
	tmCollider.q = physx::PxQuat(myRotationOffset);

	myShape->setLocalPose(tmCollider);
}

void CapsuleCollider::Start()
{
	Collider::Start();

	//SetParameters(myRadius, myHalfHeight);
}

void CapsuleCollider::DrawCollider()
{
	CU::Vector4f colour = { 0.f, 1.f, 0.f, 1.f }; // Green colour for the lines

	//DebugDrawer::Get().DrawCapsule(myPositionOffset, myRadius, myHalfHeight, colour);
}

void CapsuleCollider::SetRadius(float aRadius)
{
	myRadius = aRadius;

	//if (!myShape)
	//	return;

	myShape->setGeometry(PxCapsuleGeometry(myRadius, myHalfHeight));
}

void CapsuleCollider::SetDiameter(float aDiameter)
{
	SetRadius(aDiameter * 0.5f);
}

void CapsuleCollider::SetHalfHeight(float aHalfHeight)
{
	myHalfHeight = aHalfHeight;

	/*if (!myShape)
		return;*/

	myShape->setGeometry(PxCapsuleGeometry(myRadius, myHalfHeight));
}
void CapsuleCollider::SetFullHeight(float aFullHeight)
{
	SetHalfHeight(aFullHeight * 0.5f);
}

void CapsuleCollider::SetParameters(float aRadius, float aHalfHeight)
{
	myHalfHeight = aHalfHeight;
	myRadius = aRadius;

	/*if (!myShape)
		return;*/

	myShape->setGeometry(PxCapsuleGeometry(myRadius, myHalfHeight));
}