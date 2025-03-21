#pragma once

#include "Vector.hpp"

namespace CommonUtilities
{
	template <class T>
	class Line
	{
	public:
		// Default constructor: there is no line, the normal is the zero vector.
		Line()
		{
			myPoint0 = CommonUtilities::Vector2<T>();
			myPoint1 = CommonUtilities::Vector2<T>();
		}

		// Copy constructor.
		Line(const Line <T>& aLine)
		{
			myPoint0 = aLine.GetPoint0();
			myPoint1 = aLine.GetPoint1();
		}

		// Constructor that takes two points that define the line, the direction is aPoint1 - aPoint0.
		Line(const CommonUtilities::Vector2<T>& aPoint0, const CommonUtilities::Vector2<T>& aPoint1)
		{
			myPoint0 = aPoint0;
			myPoint1 = aPoint1;
		}

		// Init the line with two points, the same as the constructor above.
		void InitWith2Points(const CommonUtilities::Vector2<T>& aPoint0, const CommonUtilities::Vector2<T>& aPoint1)
		{
			myPoint0 = aPoint0;
			myPoint1 = aPoint1;
		}

		// Init the line with a point and a direction.
		void InitWithPointAndDirection(const CommonUtilities::Vector2<T>& aPoint, const CommonUtilities::Vector2<T>& aDirection)
		{
			myPoint0 = aPoint;
			myPoint1 = Vector2<T>(aPoint + aDirection);
		}

		// Returns the direction of the line.
		const CommonUtilities::Vector2<T> GetDirection() const
		{
			CommonUtilities::Vector2<T> retunedValue = CommonUtilities::Vector2<T>(myPoint1 - myPoint0);

			return retunedValue;
		}

		// Returns the normal of the line, which is (-direction.y, direction.x).
		const CommonUtilities::Vector2<T> GetNormal() const
		{
			CommonUtilities::Vector2<T> dirVector = GetDirection();

			return CommonUtilities::Vector2<T>(-dirVector.y, dirVector.x);
		}

		// Returns whether a point is inside the line: it is inside when the point is on the line or on the side the normal is pointing away from.
		bool IsInside(const CommonUtilities::Vector2<T>& aPosition) const
		{
			Vector2<T> vectorToTarget = aPosition - myPoint0;

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

		const CommonUtilities::Vector2<T> GetPoint0() const
		{
			return myPoint0;
		}
		const CommonUtilities::Vector2<T> GetPoint1() const
		{
			return myPoint1;
		}

		void SetPoint0(CommonUtilities::Vector2<T> aPoint)
		{
			myPoint0 = aPoint;
		}
		void SetPoint1(CommonUtilities::Vector2<T> aPoint)
		{
			myPoint1 = aPoint;
		}


	private:
		CommonUtilities::Vector2<T> myPoint0;
		CommonUtilities::Vector2<T> myPoint1;
	};
}