#pragma once

#include "Collider2D.h"

#include <Vector/Vector.hpp>

namespace non
{
	class SphereCollider2D : public Collider2D
	{
	public:
		SphereCollider2D(const float aRadius);
		~SphereCollider2D() = default;

		void SetRadius(const float aRadius);
		const float GetRadius() const;

	private:
		float myRadius;
	};
}