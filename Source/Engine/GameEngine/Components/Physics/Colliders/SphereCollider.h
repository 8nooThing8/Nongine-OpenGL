#pragma once

#include "Collider.h"

using namespace physx;

	class SphereCollider : public Collider
	{
	public:
		SphereCollider();
		SphereCollider(SphereCollider&) = default;
		~SphereCollider() override = default;

		SphereCollider(float aRadius);

		void Start() override;

		void SetRadius(float aRadius);
		void SetDiameter(float aDiameter);

	private:
		float myRadius;
	};