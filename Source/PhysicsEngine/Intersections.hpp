#pragma once

#include <Vector/Vector.hpp>
#include <Matrix/Matrix.hpp>

namespace non
{
	struct Vertex
	{
		CU::Vector3f& position;
		CU::Vector3f& normal;
	};

	struct CollisionMesh
	{
		std::vector<Vertex>& mesh;
	};

	struct SimpleBBShape
	{
		SimpleBBShape(const std::vector<CU::Vector3f>& aPoints, const CU::Vector3f& aCenter) : points(aPoints), center(aCenter) {}

		const CU::Vector3f& center;
		const std::vector<CU::Vector3f>& points;

		CU::Vector3f normals[3]
		{
			{1, 0, 0},
			{0, 1, 0},
			{0, 0, 1}
		};
	};

	struct SimpleSphereShape
	{
		SimpleSphereShape(const CU::Vector3f& aPoint, const float& aRadius) : point(aPoint), radius(aRadius)
		{
			
		}
		
		const CU::Vector3f& point;
		const float& radius;
	};

	void SATTesting(const CU::Vector3f& anAxis, const std::vector<CU::Vector3f>& aPoints, float& aMinAlong, float& aMaxAlong)
	{
		aMinAlong = std::numeric_limits<float>::max();
		aMaxAlong = std::numeric_limits<float>::min();

		for (const CU::Vector3f& point : aPoints)
		{
			const float dotValue = point.Dot(anAxis);
			aMinAlong = std::min(aMinAlong, dotValue);
			aMaxAlong = std::max(aMaxAlong, dotValue);
		}
	}

	bool OverlapOnAxis(const CU::Vector3f& axis, const SimpleBBShape& box1, const SimpleBBShape& box2, float& aOverlap)
	{
		float min1, max1, min2, max2;

		SATTesting(axis, box1.points, min1, max1);
		SATTesting(axis, box2.points, min2, max2);

		if (max1 < min2 || max2 < min1)
			return false; // No overlap, return 0 penetration

		float overlap = std::min(max1, max2) - std::max(min1, min2);

		aOverlap = std::max(overlap, aOverlap);

		return true;
	}

	float IntersectABBABB(const SimpleBBShape& boxA, const SimpleBBShape& boxB)
	{
		float totalOverlap = 0;

		for (int i = 0; i < 3; i++) 
			if (!OverlapOnAxis(boxA.normals[i], boxA, boxB, totalOverlap))
				return 0.f;

		for (int i = 0; i < 3; i++)
			if (!OverlapOnAxis(boxB.normals[i], boxA, boxB, totalOverlap))
				return 0.f;

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				CU::Vector3f axis = boxA.normals[i].Cross(boxB.normals[j]);

				if (axis.LengthSqr() < 0.001f)
					continue;

				if (!OverlapOnAxis(axis, boxA, boxB, totalOverlap))
					return 0.f;
			}
		}
		return totalOverlap;
	}

	bool IntersectSphereABB(const SimpleSphereShape& aSimpleSphere0, const SimpleBBShape& aSimpleAABB1)
	{
		//Vector3f delta = aSimpleSphere0.point - aSimpleAABB1.center;

		//// 2. Project onto OBB axes and clamp to OBB bounds
		//float distU = delta.Dot(obbAxisU);
		//distU = std::max(-obbHalfExtentU, std::min(obbHalfExtentU, distU));

		//float distV = delta.Dot(obbAxisV);
		//distV = std::max(-obbHalfExtentV, std::min(obbHalfExtentV, distV));

		//float distW = delta.Dot(obbAxisW);
		//distW = std::max(-obbHalfExtentW, std::min(obbHalfExtentW, distW));

		//// 3. Early exit if sphere center is inside OBB
		//if (std::abs(distU) <= obbHalfExtentU &&
		//	std::abs(distV) <= obbHalfExtentV &&
		//	std::abs(distW) <= obbHalfExtentW)
		//{
		//	return true;
		//}

		//// 4. Find closest point on OBB to sphere center
		//Vector3f closestPoint = obbCenter;
		//closestPoint += distU * obbAxisU;
		//closestPoint += distV * obbAxisV;
		//closestPoint += distW * obbAxisW;

		//// 5. Check if sphere contains closest point
		//float distanceSq = (sphereCenter - closestPoint).LengthSquared();
		//return (distanceSq <= sphereRadius * sphereRadius);

		return false;
	}
}