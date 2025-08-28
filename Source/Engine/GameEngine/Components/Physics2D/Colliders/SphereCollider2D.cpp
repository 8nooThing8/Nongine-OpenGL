#include "Collider2D.h"
#include "SphereCollider2D.h"

namespace non
{
	SphereCollider2D::SphereCollider2D(const float aRadius) : myRadius(aRadius) 
	{
		myShape = Shape::SphereShape;
	}

	void SphereCollider2D::SetRadius(const float aRadius)
	{
		myRadius = aRadius;
	}

	const float SphereCollider2D::GetRadius() const
	{
		return myRadius;
	}
}