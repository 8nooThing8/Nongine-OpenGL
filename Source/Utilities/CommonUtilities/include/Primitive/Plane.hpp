#pragma once

#include "Vector/Vector.hpp"

namespace CommonUtilities
{
	template <class T>
	class Plane
	{
	public:
		// Default constructor.
		Plane()
		{
			myNormal = CommonUtilities::Vector3<T>();
			myPoint = CommonUtilities::Vector3<T>();
		}

		// Constructor taking three points where the normal is (aPoint1 - aPoint0) x (aPoint2 - aPoint0).
		Plane(const CommonUtilities::Vector3<T>& aPoint0, const CommonUtilities::Vector3<T>& aPoint1, const CommonUtilities::Vector3<T>& aPoint2)
		{
			myNormal = CommonUtilities::Vector3<T>(aPoint1 - aPoint0).Cross(aPoint2 - aPoint0);

			myPoint = aPoint0;
		}

		// Constructor taking a point and a normal.
		Plane(const CommonUtilities::Vector3<T>& aPoint, const CommonUtilities::Vector3<T>& aNormal)
		{
			myPoint = aPoint;
			myNormal = aNormal;
		}

		// Init the plane with three points, the same as the constructor above.
		void InitWith3Points(const CommonUtilities::Vector3<T>& aPoint0, const CommonUtilities::Vector3<T>& aPoint1, const CommonUtilities::Vector3<T>& aPoint2)
		{
			myNormal = CommonUtilities::Vector3<T>(aPoint1 - aPoint0).Cross(aPoint2 - aPoint0);

			myPoint = aPoint0;
		}

		// Init the plane with a point and a normal, the same as the constructor above.
		void InitWithPointAndNormal(const CommonUtilities::Vector3<T>& aPoint, const CommonUtilities::Vector3<T>& aNormal)
		{
			myNormal = aNormal;
			myPoint = aPoint;
		}

		// Returns the normal of the plane
		const CommonUtilities::Vector3<T>& GetNormal() const
		{
			return myNormal;
		}

		// Returns the orgin of the plane
		const CommonUtilities::Vector3<T>& GetOrgin() const
		{
			return myPoint;
		}

		// Returns whether a point is inside the plane: it is inside when the point is on the plane or on the side the normal is pointing away from.
		bool IsInside(const CommonUtilities::Vector3<T>& aPosition) const
		{
			Vector3<T> vectorToTarget = aPosition - myPoint;

			float dotProduct = vectorToTarget.Dot(GetNormal());

			const float isInsideTolerance = 0.000001f;
			if (dotProduct < isInsideTolerance)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

	private:
		CommonUtilities::Vector3<T> myNormal;
		CommonUtilities::Vector3<T> myPoint;
	};

}