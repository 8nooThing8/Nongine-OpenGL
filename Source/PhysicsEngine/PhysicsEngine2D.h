#pragma once

#include "Vector/Vector2.hpp"

namespace non
{
	class PhysicsScene2D;

	class PhysicsEngine2D
	{
	public:
		PhysicsEngine2D();
		~PhysicsEngine2D() = default;

		static PhysicsEngine2D& Get();

		void Update(float aDeltaTime);

		const CommonUtilities::Vector2f& GetGravity();
		void SetGravity(const CommonUtilities::Vector2f& aGravity);

		PhysicsScene2D* GetPhysicsEngine();

	private:

		PhysicsScene2D* myCurrentPhysicsScene;
		CommonUtilities::Vector2f myGravity;
	};
}
 