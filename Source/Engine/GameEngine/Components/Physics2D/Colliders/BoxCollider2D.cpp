#include "Collider2D.h"
#include "BoxCollider2D.h"

namespace non
{
	BoxCollider2D::BoxCollider2D(const CU::Vector2f& aHalfExtents) : myHalfExtents(aHalfExtents * 0.75f) 
	{
		myShape = Shape::BoxShape;
	}

	void BoxCollider2D::SetHalfExtents(const CU::Vector2f& aHalfExtents)
	{
		myHalfExtents = aHalfExtents;
	}

	const CU::Vector2f BoxCollider2D::GetHalfExtents() const
	{
		return myHalfExtents;
	}
}

