#pragma once

#include "Shape.hpp"
#include "Vector/Vector.hpp"

namespace CommonUtilities
{
	class AABB3D : public Shape
	{
	public:
		AABB3D() : myHalfExtent(0, 0, 0) {}
		~AABB3D() = default;

		AABB3D(const AABB3D& anAABB) : myHalfExtent(anAABB.myHalfExtent) {}
		AABB3D(float aX, float aY, float aZ) : myHalfExtent(aX, aY, aZ) {}
		AABB3D(const CommonUtilities::Vector3<float>& aHalfExtent) : myHalfExtent(aHalfExtent) {}

		bool IsInside(const CommonUtilities::Vector3<float>& aPosition) const
		{
			return aPosition <= myPosition + myHalfExtent && myPosition - myHalfExtent <= aPosition;
		}

		void SetHalfExtents(CommonUtilities::Vector3<float> aHalfExtent)
		{
			myHalfExtent = aHalfExtent;
		}

		const CommonUtilities::Vector3<float>& GetHalfExtents() const
		{
			return myHalfExtent;
		}

	private:
		CommonUtilities::Vector3<float> myHalfExtent;
	};
}