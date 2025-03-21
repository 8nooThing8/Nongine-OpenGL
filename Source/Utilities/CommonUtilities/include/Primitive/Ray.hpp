#pragma once

#include "vector/Vector.hpp"

namespace CommonUtilities
{
	template <class T>
	class Ray
	{
	public:
		~Ray() = default;

		// Default constructor: there is no ray, the origin and direction are the
		// zero vector.
		Ray()
		{
			myOrgin = CommonUtilities::Vector3<T>();
			myDirection = CommonUtilities::Vector3<T>();
		}

		// Copy constructor.
		Ray(const Ray<T>& aRay)
		{
			myOrgin = aRay.GetOrigin();
			myDirection = aRay.GetDirection();
		}

		// Constructor that takes origin and direction to define the ray
		Ray(const Vector3<T>& aOrigin, const Vector3<T>& aDirection)
		{
			myOrgin = aOrigin;
			myDirection = aDirection.GetNormalized();
		}

		// Init the ray with two points.
		void InitWith2Points(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1)
		{
			myOrgin = aPoint0;
			myDirection = aPoint1 - aPoint0;
		}

		// Init the ray with an origin and a direction.
		void InitWithOriginAndDirection(const Vector3<T>& aOrigin, const Vector3<T>& aDirection)
		{
			myOrgin = aOrigin;
			myDirection = aDirection;
		}

		// Get the orgin of the ray
		const CommonUtilities::Vector3<T> GetOrigin() const
		{
			return myOrgin;
		}

		// Get the direction of the ray
		const CommonUtilities::Vector3<T> GetDirection() const
		{
			return myDirection;
		}

	private:
		CommonUtilities::Vector3<T> myOrgin;
		CommonUtilities::Vector3<T> myDirection;
	};
}