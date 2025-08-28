#pragma once

#include "Collider2D.h"

#include <Vector/Vector.hpp>

namespace non
{
	class BoxCollider2D : public Collider2D
	{
	public:
		BoxCollider2D(const CU::Vector2f& aHalfExtents);
		~BoxCollider2D() = default;

		void SetHalfExtents(const CU::Vector2f& aHalfExtents);
		const CU::Vector2f GetHalfExtents() const;

	private:
		CU::Vector2f myHalfExtents;
	};
}