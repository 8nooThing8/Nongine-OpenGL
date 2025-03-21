#pragma once

#include "vector/Vector.hpp"

namespace CommonUtilities
{
	template <class T>
	class Sphere
	{
	public:
		// Default constructor: there is no sphere, the radius is zero and the position is
		// the zero vector
		Sphere()
		{
			mySphereRadius = 0;
			mySpherePosition = CommonUtilities::Vector3<float>();
		}

		~Sphere() = default;

		// Copy constructor
		Sphere(const CommonUtilities::Sphere<T>& aSphere)
		{
			mySpherePosition = aSphere.GetSpherePosition();
			mySphereRadius = aSphere.GetRadius();
		}

		// Constructor that takes the center position and radius of the sphere
		Sphere(const CommonUtilities::Vector3<T>& aCenter, T aRadius)
		{
			mySpherePosition = aCenter;
			mySphereRadius = aRadius;
		}

		// Init the sphere with a center and a radius, the same as the constructor above
		void InitWithCenterAndRadius(const CommonUtilities::Vector3<T>& aCenter, T aRadius)
		{
			mySpherePosition = aCenter;
			mySphereRadius = aRadius;
		}

		// Returns whether a point is inside the sphere: it is inside when the point is on the
		// sphere surface or inside of the sphere
		bool IsInside(const CommonUtilities::Vector3<T>& aPosition) const
		{
			return CommonUtilities::Distance(aPosition, mySpherePosition) <= mySphereRadius;
		}

		const T GetRadius() const
		{
			return mySphereRadius;
		}

		const CommonUtilities::Vector3<T> GetSpherePosition() const
		{
			return mySpherePosition;
		}


	private:
		CommonUtilities::Vector3<T> mySpherePosition;
		T mySphereRadius;
	};
}
