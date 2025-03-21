#include "../Engine/Engine.pch.h"

#include "BoxCollider.h"

#include "../Engine/GameEngine/MainSingleton.h"
#include "../Engine/GameEngine/SceneManagerInternal.h"
#include "../Engine/GameEngine/Components/GameObject.h"

BoxCollider::BoxCollider(const CU::Vector3f& aHalfExtents) : Collider(PhysicsEngine::Get().CreateBoxCollider(aHalfExtents)), myHalfExtents(aHalfExtents)
{
}

BoxCollider::BoxCollider() : Collider(PhysicsEngine::Get().CreateBoxCollider(CommonUtilities::Vector3f(1, 1, 1))), myHalfExtents(CommonUtilities::Vector3f(1, 1, 1))
{
	myCollisionLayer = CollisionLayer::Default;
}

void BoxCollider::Start()
{
	SetHalfExtents(myHalfExtents * gameObject->transform->scale.ToVector3());

	Collider::Start();

	//SetHalfExtents(myHalfExtents);
}

void BoxCollider::RenderImGUI()
{
	ImGui::DragFloat3("Half extents: ", &myHalfExtents[0], 0.1f);

	Collider::RenderImGUI();
}

//void BoxCollider::DrawCollider()
//{
//	auto& engine = GraphicsEngine::Get();
//	CU::Vector4f colour = { 0.f, 1.f, 0.f, 1.f }; // Green colour for the lines

//	PxVec3 min;
//	PxVec3 max;

//	ComputeRawBounds(min, max);

//	// Define the 8 corners of the box
//	CU::Vector3f corners[8] = {
//		{min.x, min.y, min.z}, // Corner 0
//		{max.x, min.y, min.z}, // Corner 1
//		{min.x, max.y, min.z}, // Corner 2
//		{max.x, max.y, min.z}, // Corner 3
//		{min.x, min.y, max.z}, // Corner 4
//		{max.x, min.y, max.z}, // Corner 5
//		{min.x, max.y, max.z}, // Corner 6
//		{max.x, max.y, max.z}  // Corner 7
//	};

//	CU::Vector3f realScale = myHalfExtents * gameObject->transform->scale.ToEuler();

//	CU::Matrix4x4f newScale = CU::Matrix4x4f::CreateScale(CU::Vector3f(realScale.z, realScale.y, realScale.x));

//	CU::Vector3f myColliderRotationEuler = myRotationOffset.ToEuler();

//	CU::Vector3f oneDivScale = CU::Vector3f(1 / realScale.x, 1 / realScale.y, 1 / realScale.z);

//	CU::Vector3f rotationOffset = CU::Vector3f(0, 270, 0) * CU::Deg2Rad;

//	CU::Matrix4x4f colliderTransform = newScale * CU::Matrix4x4f::CreateTransformation(myPositionOffset, myColliderRotationEuler + rotationOffset, oneDivScale);

//	CU::Matrix4x4f objectMAtrix = gameObject->GetTransform().GetMatrixNoScale();

//	for (int i = 0; i < 8; ++i)
//	{
//		corners[i] = corners[i] * (colliderTransform * objectMAtrix);
//	}

//	LinePrimitive lines[12];

//	// Bottom face
//	lines[0].myStartPoint = { corners[0].ToVector4(1.f), colour };
//	lines[0].myEndPoint = { corners[1].ToVector4(1.f), colour };

//	lines[1].myStartPoint = { corners[1].ToVector4(1.f), colour };
//	lines[1].myEndPoint = { corners[3].ToVector4(1.f), colour };

//	lines[2].myStartPoint = { corners[3].ToVector4(1.f), colour };
//	lines[2].myEndPoint = { corners[2].ToVector4(1.f), colour };

//	lines[3].myStartPoint = { corners[2].ToVector4(1.f), colour };
//	lines[3].myEndPoint = { corners[0].ToVector4(1.f), colour };

//	// Top face
//	lines[4].myStartPoint = { corners[4].ToVector4(1.f), colour };
//	lines[4].myEndPoint = { corners[5].ToVector4(1.f), colour };

//	lines[5].myStartPoint = { corners[5].ToVector4(1.f), colour };
//	lines[5].myEndPoint = { corners[7].ToVector4(1.f), colour };

//	lines[6].myStartPoint = { corners[7].ToVector4(1.f), colour };
//	lines[6].myEndPoint = { corners[6].ToVector4(1.f), colour };

//	lines[7].myStartPoint = { corners[6].ToVector4(1.f), colour };
//	lines[7].myEndPoint = { corners[4].ToVector4(1.f), colour };

//	// Connecting lines between top and bottom faces
//	lines[8].myStartPoint = { corners[0].ToVector4(1.f), colour };
//	lines[8].myEndPoint = { corners[4].ToVector4(1.f), colour };

//	lines[9].myStartPoint = { corners[1].ToVector4(1.f), colour };
//	lines[9].myEndPoint = { corners[5].ToVector4(1.f), colour };

//	lines[10].myStartPoint = { corners[2].ToVector4(1.f), colour };
//	lines[10].myEndPoint = { corners[6].ToVector4(1.f), colour };

//	lines[11].myStartPoint = { corners[3].ToVector4(1.f), colour };
//	lines[11].myEndPoint = { corners[7].ToVector4(1.f), colour };

//	for (int i = 0; i < 12; ++i)
//	{
//		engine.AddLine(lines[i]);
//	}

//}

void BoxCollider::SetHalfExtents(const CU::Vector3f& aHalfExtents)
{
	myHalfExtents = aHalfExtents;

	//if (!myShape)
	//	return;

	myShape->setGeometry(PxBoxGeometry(physx::PxVec3(myHalfExtents)));
}

void BoxCollider::SetFullExtents(const CU::Vector3f& aFullExtents)
{
	SetHalfExtents((aFullExtents * 0.5f));
}

void BoxCollider::SetParameters(const CU::Vector3f& aHalfExtents)
{
	SetHalfExtents(aHalfExtents);
}

CommonUtilities::Vector3f& BoxCollider::GetHalfExtents()
{
	return myHalfExtents;
}
