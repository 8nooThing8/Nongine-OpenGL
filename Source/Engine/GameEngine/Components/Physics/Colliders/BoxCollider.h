#pragma once

#include "Collider.h"

using namespace physx;

class BoxCollider : public Collider
{
public:
	BoxCollider();
	~BoxCollider() override = default;
	BoxCollider(BoxCollider&) = default;
	

	BoxCollider(const CU::Vector3f& aHalfExtents);

	void Start() override;


	//void DrawCollider() override;

	void SetHalfExtents(const CU::Vector3f& aHalfExtents);
	void SetFullExtents(const CU::Vector3f& aFullExtents);

	void SetParameters(const CU::Vector3f& aHalfExtents);

	CommonUtilities::Vector3f& GetHalfExtents();

	void RenderImGUI() override;

private:
	//void ComputeRawBounds(physx::PxVec3& outMin, physx::PxVec3& outMax);

	CU::Vector3f myHalfExtents;
};