#pragma once

#include "Vector/Vector.hpp"

#include <vector>

#include <assert.h>

#include <array>

namespace CommonUtilities
{
	template <class T>
	class AABB3D
	{
	public:
		~AABB3D() = default;

		// Default constructor: there is no AABB, both min and max points are the zero vector.
		AABB3D()
		{
			myMin = CommonUtilities::Vector3<T>();
			myMax = CommonUtilities::Vector3<T>();
		}

		// Copy constructor.
		AABB3D(const AABB3D<T>& aAABB3D)
		{
			myMin = aAABB3D.GetMin();
			myMax = aAABB3D.GetMax();
		}

		// Constructor taking the positions of the minimum and maximum corners.
		AABB3D(const CommonUtilities::Vector3<T>& aMin, const CommonUtilities::Vector3<T>& aMax)
		{
			myMin = aMin;
			myMax = aMax;
		}

		// Init the AABB with the positions of the minimum and maximum corners, same as
		// the constructor above.
		void InitWithMinAndMax(const CommonUtilities::Vector3<T>& aMin, const CommonUtilities::Vector3<T>& aMax)
		{
			myMin = aMin;
			myMax = aMax;
		}
		// Returns whether a point is inside the AABB: it is inside when the point is on any
		// of the AABB's sides or inside of the AABB.
		bool IsInside(const CommonUtilities::Vector3<T>& aPosition) const
		{
			bool isInside = false;

			isInside = aPosition <= myMax && myMin <= aPosition;

			return isInside;
		}

		// Get the min position
		const CommonUtilities::Vector3<T> GetMin() const
		{
			return myMin;
		}

		// Get the max position
		const CommonUtilities::Vector3<T> GetMax() const
		{
			return myMax;
		}

	private:
		CommonUtilities::Vector3<T> myMin;
		CommonUtilities::Vector3<T> myMax;
	};
}