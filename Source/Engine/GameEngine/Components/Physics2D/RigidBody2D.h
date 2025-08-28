#pragma once

#include <vector>

#include "Components/Component.h"
#include "Vector/Vector2.hpp"

namespace non
{
	class RigidBody2D : public Component
	{
	public:
		friend class PhysicsScene2D;

		RigidBody2D(float aDrag) : myDrag(aDrag), myVelocity({ 0, 0 }) {};
		~RigidBody2D() = default;

		void Start() override;

		const CommonUtilities::Vector2f& GetVelocity() const;
		CommonUtilities::Vector2f& GetVelocity();
		void SetVelocity(const CommonUtilities::Vector2f& aVelocity);

		float GetDrag();
		void SetDrag(float aDrag);

	private:
		float myDrag;
		CommonUtilities::Vector2f myVelocity;
	};
}