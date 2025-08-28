#pragma once

#include "Shape.hpp"
#include <Vector/Vector.hpp>

namespace CommonUtilities
{
	class Sphere : public Shape
	{
	public:
		Sphere() : mySphereRadius(0){}
		~Sphere() = default;

		Sphere(const Sphere& aSphere) : mySphereRadius(aSphere.mySphereRadius) {}
		Sphere(float aRadius) : mySphereRadius(aRadius) {}

		bool IsInside(const CommonUtilities::Vector3<float>& aPosition) const
		{
			float radiusSqrd = mySphereRadius * mySphereRadius;
			return CommonUtilities::DistanceSqrd(aPosition, myPosition) <= radiusSqrd;
		}

		const float GetRadius() const
		{
			return mySphereRadius;
		}

		void SetRadius(float aRadius) 
		{ 
			mySphereRadius = aRadius; 
		}

	private:
		float mySphereRadius;
	};
}
