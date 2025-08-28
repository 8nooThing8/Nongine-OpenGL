#pragma once

#include "Components/Component.h"

#include <Vector/Vector.hpp>

namespace non
{
	enum Shape
	{
		null,
		SphereShape,
		BoxShape
	};

	class Collider2D : public Component
	{
	public:
		Collider2D() : myRotation(0), myPosition(), myShape(null) {};
		~Collider2D() = default;

		void Start() override;

		void OnTransform() override;

		void SetPosition(const CU::Vector2f& aPosition);
		const CU::Vector2f& GetPosition() const;

		void SetRotation(float aRotation);
		const float GetRotation() const;

		Shape myShape;
	
	private:
		CU::Vector2f myPosition;
		float myRotation;

	};
}