#include "Collider2D.h"

#include "PhysicsEngine/PhysicsEngine2D.h"
#include "PhysicsEngine/PhysicsScene2D.h"

#include "Components/GameObject.h"

namespace non
{
	void Collider2D::Start()
	{
		OnTransform();

		PhysicsScene2D& physScene = *PhysicsEngine2D::Get().GetPhysicsEngine();
		physScene.AddCollider(this);
	}

	void Collider2D::OnTransform()
	{
		std::memcpy(&myPosition, &gameObject->transform->GetPosition(), sizeof(float) * 2);
		myRotation = gameObject->transform->GetRotation().z;
	}

	void Collider2D::SetPosition(const CU::Vector2f& aPosition)
	{
		myPosition = aPosition;
	}

	void Collider2D::SetRotation(float aRotation)
	{
		myRotation = aRotation;
	}

	const CU::Vector2f& Collider2D::GetPosition() const
	{
		return myPosition;
	}

	const float Collider2D::GetRotation() const
	{
		return myRotation;
	}
}