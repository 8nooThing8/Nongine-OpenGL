#pragma once

#include "Plane.hpp"
#include "Ray.hpp"
#include "AABB3D.hpp"
#include "Sphere.hpp"
#include "UtilityFunctions.hpp"

#include <iostream>

#include <vector>

namespace CommonUtilities
{
	// If the ray is parallel to the plane, aOutIntersectionPoint remains unchanged. If
	// the ray is in the plane, true is returned, if not, false is returned. If the ray
	// isn't parallel to the plane and hits, the intersection point is stored in
	// aOutIntersectionPoint and true returned otherwise false is returned and
	// outIntersectionPoint is unchanged.
	template <class T>
	bool IntersectionPlaneRay(const Plane<T>& aPlane, const Ray<T>& aRay, CommonUtilities::Vector3<T>& outIntersectionPoint)
	{
		Vector3<T> distanceFromEndRayIntersectionDistanceoPlane = aPlane.GetOrgin() - aRay.GetOrigin();

		T numerator = distanceFromEndRayIntersectionDistanceoPlane.Dot(aPlane.GetNormal());
		T denumerator = aRay.GetDirection().Dot(aPlane.GetNormal());

		const float epsilon = std::numeric_limits<float>::epsilon();
		if (std::abs(denumerator) < epsilon)
		{
			if (std::abs(numerator) < epsilon)
			{
				outIntersectionPoint = aRay.GetOrigin();
				return true;
			}

			return false;
		}

		T t = numerator / denumerator;

		if (t >=0)
		{
			outIntersectionPoint = aRay.GetOrigin() + aRay.GetDirection().GetNormalized() * t;
		}
		else
		{
			outIntersectionPoint = CommonUtilities::Vector3<float>();
		}

		return (t >= 0);
	}

	// If no collision, aOutIntersectionPoint remains unchanged.
	// If The sphere overlaps the AABB true is returned, if not, false is returned.
	// The point on the AABB closest to the sphere centre is saved in
	// aOutIntersectionPoint.
	// A sphere touching the aabb is considered overlapping.
	template <class T>
	bool IntersectionSphereAABB(const Sphere<T>& aSphere, const AABB3D<T>& aAABB3D, CommonUtilities::Vector3<T>& outIntersectionPoint)
	{
		outIntersectionPoint.x = CommonUtilities::Clamp(aSphere.GetSpherePosition().x, aAABB3D.GetMin().x, aAABB3D.GetMax().x);
		outIntersectionPoint.y = CommonUtilities::Clamp(aSphere.GetSpherePosition().y, aAABB3D.GetMin().y, aAABB3D.GetMax().y);
		outIntersectionPoint.z = CommonUtilities::Clamp(aSphere.GetSpherePosition().z, aAABB3D.GetMin().z, aAABB3D.GetMax().z);

		float distanceFromMidPointToSphere = CommonUtilities::Distance(aSphere.GetSpherePosition(), outIntersectionPoint);

		bool isColliding = distanceFromMidPointToSphere <= aSphere.GetRadius();

		if (!isColliding)
		{
			outIntersectionPoint.x = 0;
			outIntersectionPoint.y = 0;
			outIntersectionPoint.z = 0;
		}

		return isColliding;
	}

	// If the ray intersects the AABB, true is returned, if not, false is returned.
	// A ray in one of the AABB's sides is counted as intersecting it.
	template <class T>
	bool IntersectionAABBRay(const AABB3D<T>& aAABB, const Ray<T>& aRay/*, CommonUtilities::Vector3<T>& outIntersectionPoint*/)
	{
		const T intersectionScalar = sizeof(float);

		CommonUtilities::Vector3<T> outIntersectionPoint;

		bool inside = true;

		Vector3<T> AABBMax = aAABB.GetMax();
		Vector3<T> AABBMin = aAABB.GetMin();

		Vector3<T> rayOrigin = aRay.GetOrigin();
		Vector3<T> rayDirection = aRay.GetDirection() * intersectionScalar;

		T xIntersectionDistance, xIntersectionNormal = static_cast<T>(0);
		if (rayOrigin.x < AABBMin.x)
		{
			xIntersectionDistance = (AABBMin.x - rayOrigin.x) / rayDirection.x;

			if (xIntersectionDistance > 1.0)
			{
				return false;
			}

			inside = false;
			xIntersectionNormal = -1.0;
		}
		else if (rayOrigin.x > AABBMax.x)
		{
			xIntersectionDistance = (AABBMax.x - rayOrigin.x) / rayDirection.x;

			if (xIntersectionDistance < 0.0)
			{
				return false;
			}

			inside = false;
			xIntersectionNormal = 1.0;
		}
		else
		{
			xIntersectionDistance = -1.0;
		}

		T yIntersectionDistance, yIntersectionNormal = static_cast<T>(0);
		if (rayOrigin.y < AABBMin.y)
		{
			yIntersectionDistance = (AABBMin.y - rayOrigin.y) / rayDirection.y;

			if (yIntersectionDistance > 1.0)
			{
				return false;
			}

			inside = false;
			yIntersectionNormal = -1.0;
		}
		else if (rayOrigin.y > AABBMax.y)
		{
			yIntersectionDistance = (AABBMax.y - rayOrigin.y) / rayDirection.y;

			if (yIntersectionDistance < 0.0)
			{
				return false;
			}

			inside = false;
			yIntersectionNormal = 1.0;
		}
		else
		{
			yIntersectionDistance = -1.0;
		}

		T zIntersectionDistance, zIntersectionNormal = static_cast<T>(0);
		if (rayOrigin.z < AABBMin.z)
		{
			zIntersectionDistance = (AABBMin.z - rayOrigin.z) / rayDirection.z;

			if (zIntersectionDistance > 1.0)
			{
				return false;
			}

			inside = false;
			zIntersectionNormal = -1.0;
		}
		else if (rayOrigin.z > AABBMax.z)
		{
			zIntersectionDistance = (AABBMax.z - rayOrigin.z) / rayDirection.z;

			if (zIntersectionDistance < 0.0)
			{
				return false;
			}

			inside = false;
			zIntersectionNormal = 1.0;
		}
		else
		{
			zIntersectionDistance = -1.0;
		}

		Vector3<T> returnNormal;

		if (inside)
		{
			outIntersectionPoint = rayOrigin;
			return true;
		}

		int which = 0;
		T t = xIntersectionDistance;
		if (yIntersectionDistance > t)
		{
			which = 1;
			t = yIntersectionDistance;
		}
		if (zIntersectionDistance > t)
		{
			which = 2;
			t = zIntersectionDistance;
		}

		switch (which)
		{
		case 0: // X-axis
		{
			T y = rayOrigin.y + rayDirection.y * t;

			if (y < AABBMin.y || y > AABBMax.y)
			{
				return false;
			}

			T z = rayOrigin.z + rayDirection.z * t;
			if (z < AABBMin.z || z > AABBMax.z)
			{
				return false;
			}

			returnNormal.x = xIntersectionNormal;
			returnNormal.y = 0.0;
			returnNormal.z = 0.0;
		}
		break;

		case 1: // Y-axis
		{
			T x = rayOrigin.x + rayDirection.x * t;
			if (x < AABBMin.x || x > AABBMax.x)
			{
				return false;
			}

			T z = rayOrigin.z + rayDirection.z * t;
			if (z < AABBMin.z || z > AABBMax.z)
			{
				return false;
			}

			returnNormal.x = 0.0;
			returnNormal.y = yIntersectionNormal;
			returnNormal.z = 0.0;
		}
		break;

		case 2: // Z-axis
		{
			T x = rayOrigin.x + rayDirection.x * t;
			if (x < AABBMin.x || x > AABBMax.x)
			{
				return false;
			}

			T y = rayOrigin.y + rayDirection.y * t;
			if (y < AABBMin.y || y > AABBMax.y)
			{
				return false;
			}

			returnNormal.x = 0.0;
			returnNormal.y = 0.0;
			returnNormal.z = zIntersectionNormal;
		}
		break;
		}

		outIntersectionPoint = rayOrigin + rayDirection * t;

		return true;
	}

	// If the ray intersects the sphere, true is returned, if not, false is returned.
	// A ray intersecting the surface of the sphere is considered as intersecting it.
	template <class T>
	bool IntersectionSphereRay(const Sphere<T>& aSphere, const Ray<T>& aRay/*, CommonUtilities::Vector3<T>& outIntersectionPoint*/)
	{
		CommonUtilities::Vector3<T> outIntersectionPoint;

		Vector3<T> distanceToSphere = aSphere.GetSpherePosition() - aRay.GetOrigin();

		T dotProduct = distanceToSphere.Dot(aRay.GetDirection().GetNormalized());

		T distSquared = distanceToSphere.LengthSqr();

		T args = static_cast<T>(std::pow(aSphere.GetRadius(), 2) - distSquared + std::pow(dotProduct, 2));

		if (args < 0 || distSquared < std::pow(aSphere.GetRadius(), 2))
		{
			outIntersectionPoint = aRay.GetOrigin();
			return false;
		}

		T t = dotProduct - std::sqrt(args);

		if (t < 0)
		{
			return false;
		}

		outIntersectionPoint = aRay.GetOrigin() + (aRay.GetDirection().GetNormalized() * t);

		return true;
	}
}