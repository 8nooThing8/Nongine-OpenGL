#include "../Engine/Engine.pch.h"

#include "SphereCollider.h"

#include "../Engine/GameEngine/MainSingleton.h"
#include "../Engine/GameEngine/SceneManagerInternal.h"
#include "../Engine/GameEngine/Components/GameObject.h"

SphereCollider::SphereCollider(float aRadius) : Collider(PhysicsEngine::Get().CreateSphereCollider(aRadius)), myRadius(aRadius)
{
	myCollisionLayer = CollisionLayer::Default;
}

SphereCollider::SphereCollider() : Collider(PhysicsEngine::Get().CreateSphereCollider(1))
{
	myCollisionLayer = CollisionLayer::Default;
}

void SphereCollider::Start()
{
	Collider::Start();
}

void SphereCollider::SetRadius(float aRadius)
{
	myRadius = aRadius;

	/*if (!myShape)
		return;*/

	myShape->setGeometry(PxSphereGeometry(myRadius));
}

void SphereCollider::SetDiameter(float aDiameter)
{
	SetRadius(aDiameter * 0.5f);
}