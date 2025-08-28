#pragma once

#ifndef INTERSECTION
#define INTERSECTION

#include "Shape.hpp"
#include "Plane.hpp"
#include "Ray.hpp"
#include "AABB3D.hpp"
#include "Sphere.hpp"
#include "UtilityFunctions.hpp"

#include <iostream>

#include <vector>

namespace CommonUtilities
{
	struct CollisionResult
	{
		bool operator==(bool aCollided)
		{
			return colliding == aCollided;
		}

		explicit operator bool() const 
		{
			return colliding;
		}

		bool colliding;
		CU::Vector3f penDepth;
		CU::Vector3f intersectionPoint;
	};

	// If the ray is parallel to the plane, aOutIntersectionPoint remains unchanged. If
	// the ray is in the plane, true is returned, if not, false is returned. If the ray
	// isn't parallel to the plane and hits, the intersection point is stored in
	// aOutIntersectionPoint and true returned otherwise false is returned and
	// outIntersectionPoint is unchanged.
	template <class T>
	inline bool IntersectionPlaneRay(const Plane<T>& aPlane, const Ray<T>& aRay, CommonUtilities::Vector3<T>& outIntersectionPoint)
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

		if (t >= 0)
		{
			outIntersectionPoint = aRay.GetOrigin() + aRay.GetDirection() * t;
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
	// aOutIntersectionPoint
	// A sphere touching the aabb is considered overlapping.

	inline CollisionResult IntersectionSphereAABB(const Sphere& aSphere, const AABB3D& aAABB3D)
	{
		CollisionResult result{ false, {0, 0, 0}, {0, 0, 0} };

		const CommonUtilities::Vector3f& aabbposition = aAABB3D.GetPosition();
		const CommonUtilities::Vector3f& aabbhalfextent = aAABB3D.GetHalfExtents();

		CommonUtilities::Vector3f min = aabbposition - aabbhalfextent;
		CommonUtilities::Vector3f max = aabbposition + aabbhalfextent;

		CommonUtilities::Vector3f sphereposition = aSphere.GetPosition();

		result.intersectionPoint.x = std::clamp(sphereposition.x, min.x, max.x);
		result.intersectionPoint.y = std::clamp(sphereposition.y, min.y, max.y);
		result.intersectionPoint.z = std::clamp(sphereposition.z, min.z, max.z);

		const CU::Vector3f sphereToIntersect = sphereposition - result.intersectionPoint;

		const float distanceFromMidPointToSphere = std::sqrt(sphereToIntersect.x * sphereToIntersect.x + sphereToIntersect.y * sphereToIntersect.y + sphereToIntersect.z * sphereToIntersect.z);

		float sphereRadius = aSphere.GetRadius();

		result.colliding = distanceFromMidPointToSphere <= sphereRadius;

		if (!result.colliding)
		{
			result.intersectionPoint = CU::Vector3f{ 0, 0, 0 };
			result.penDepth = CU::Vector3f{0, 0, 0};

			return result;
		}

		float penetrationDepth = sphereRadius - distanceFromMidPointToSphere;

		if (distanceFromMidPointToSphere > 0.0f)
			result.penDepth = sphereToIntersect * (penetrationDepth / distanceFromMidPointToSphere);
		else
			result.penDepth = CU::Vector3f{ 0, 1.0f, 0 } *sphereRadius;

		return result;
	}

	inline CollisionResult IntersectionAABBAABB(const AABB3D& aAABB3Da, const AABB3D& aAABB3Db)
	{
		CollisionResult result{ false, {0, 0, 0}, {0, 0, 0} };

		const CommonUtilities::Vector3f& aabbPositiona = aAABB3Da.GetPosition();
		const CommonUtilities::Vector3f& aabbHalfextenta = aAABB3Da.GetHalfExtents();

		const CommonUtilities::Vector3f& aabbPositionb = aAABB3Db.GetPosition();
		const CommonUtilities::Vector3f& aabbHalfextentb = aAABB3Db.GetHalfExtents();

		CommonUtilities::Vector3f mina = aabbPositiona - aabbHalfextenta;
		CommonUtilities::Vector3f maxa = aabbPositiona + aabbHalfextenta;

		CommonUtilities::Vector3f minb = aabbPositionb - aabbHalfextentb;
		CommonUtilities::Vector3f maxb = aabbPositionb + aabbHalfextentb;

		if (maxa.x < minb.x || maxb.x < mina.x) return result;
		if (maxa.y < minb.y || maxb.y < mina.y) return result;
		if (maxa.z < minb.z || maxb.z < mina.z) return result;

		result.penDepth.x = (aabbHalfextenta.x + aabbHalfextentb.x) - fabs(aabbPositiona.x - aabbPositionb.x);
		result.penDepth.y = (aabbHalfextenta.y + aabbHalfextentb.y) - fabs(aabbPositiona.y - aabbPositionb.y);
		result.penDepth.z = (aabbHalfextenta.z + aabbHalfextentb.z) - fabs(aabbPositiona.z - aabbPositionb.z);

		float intersectionMinX = fmax(mina.x, minb.x);
		float intersectionMaxX = fmin(mina.x, minb.x);

		float intersectionMinY = fmax(mina.y, minb.y);
		float intersectionMaxY = fmin(mina.y, minb.y);

		float intersectionMinZ = fmax(mina.z, minb.z);
		float intersectionMaxZ = fmin(mina.z, minb.z);

		result.colliding = true;
		result.intersectionPoint.x = (intersectionMinX + intersectionMaxX) * 0.5f;
		result.intersectionPoint.y = (intersectionMinY + intersectionMaxY) * 0.5f;
		result.intersectionPoint.z = (intersectionMinZ + intersectionMaxZ) * 0.5f;

		return result;
	}

	inline CollisionResult IntersectionSphereSphere(const Sphere& aSpherea, const Sphere& aSphereb)
	{
		CollisionResult result{ false, {0, 0, 0}, {0, 0, 0} };

		const CommonUtilities::Vector3f& SpherePositionA = aSpherea.GetPosition();
		float sphereRadiusA = aSpherea.GetRadius();

		const CommonUtilities::Vector3f& SpherePositionB = aSphereb.GetPosition();
		float sphereRadiusB = aSphereb.GetRadius();

		float distanceSqrd = CU::DistanceSqrd(SpherePositionA, SpherePositionB);
		
		float radSqrdA = sphereRadiusA * sphereRadiusA;
		float radSqrdB = sphereRadiusB * sphereRadiusB;

		if (distanceSqrd > radSqrdA + radSqrdB)
			return result;

		result.colliding = true;

		return result;
	}

	// If the ray intersects the AABB, true is returned, if not, false is returned.
	// A ray in one of the AABB's sides is counted as intersecting it.
	template <class T>
	inline bool IntersectionAABBRay(const AABB3D& aAABB, const Ray<T>& aRay, T aDistance)
	{
		bool inside = true;

		Vector3<T> AABBMax = aAABB.GetPosition() + aAABB.GetHalfExtents();
		Vector3<T> AABBMin = aAABB.GetPosition() - aAABB.GetHalfExtents();

		Vector3<T> rayOrigin = aRay.GetOrigin();
		Vector3<T> rayDirection = aRay.GetDirection();

		T tMin = 0.0f;
		T tMax = aDistance;

		for (int i = 0; i < 3; i++) {
			if (fabs(rayDirection[i]) < std::numeric_limits<T>::epsilon()) 
			{
				if (rayOrigin[i] < AABBMin[i] || rayOrigin[i] > AABBMax[i]) 
				{
					return false;
				}
			}
			else 
			{
				T invDir = 1.0f / rayDirection[i];
				T t1 = (AABBMin[i] - rayOrigin[i]) * invDir;
				T t2 = (AABBMax[i] - rayOrigin[i]) * invDir;

				if (t1 > t2) std::swap(t1, t2);

				tMin = t1 > tMin ? t1 : tMin;
				tMax = t2 < tMax ? t2 : tMax;

				if (tMin > tMax) 
				{
					return false;
				}
			}
		}

		// If we get here, there's an intersection
		CommonUtilities::Vector3<T> outIntersectionPoint = rayOrigin + rayDirection * tMin;
		return true;
	}

	// If the ray intersects the sphere, true is returned, if not, false is returned.
	// A ray intersecting the surface of the sphere is considered as intersecting it.
	template <class T>
	inline bool IntersectionSphereRay(const Sphere& aSphere, const Ray<T>& aRay/*, CommonUtilities::Vector3<T>& outIntersectionPoint*/)
	{
		const Vector3<T> distanceToSphere = aSphere.GetPosition() - aRay.GetOrigin();

		const T dotProduct = distanceToSphere.Dot(aRay.GetDirection());

		const T distSquared = distanceToSphere.LengthSqr();

		const float radius = aSphere.GetRadius();
		const float radiusSqrd = radius * radius;

		const T args = static_cast<T>(radiusSqrd - distSquared + radiusSqrd);

		CommonUtilities::Vector3<T> outIntersectionPoint;

		if (args < 0 || distSquared < radiusSqrd)
		{
			outIntersectionPoint = aRay.GetOrigin();
			return false;
		}

		const T t = dotProduct - std::sqrt(args);

		if (t < 0)
		{
			return false;
		}

		outIntersectionPoint = aRay.GetOrigin() + (aRay.GetDirection() * t);

		return true;
	}
}
#endif // !INTERSECTION