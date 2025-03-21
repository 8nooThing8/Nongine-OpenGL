#include "../Engine/Engine.pch.h"

#include "Collider.h"

#include "../Engine/GameEngine/Components/Physics/CharacterController.h"
#include "../Engine/GameEngine/Components/Physics/RigidBody.h"
#include "../Engine/GameEngine/Components/GameObject.h"

#include "Quaternion/Quaternion.hpp"

#include "../Engine/GameEngine/MainSingleton.h"
#include "../Engine/GameEngine/SceneManagerInternal.h"

using namespace physx;

Collider::Collider(PxShape* aShape)
{
	Component();

	myShape = aShape;
}

void Collider::SetCollisionLayer(CollisionLayer aCollisionLayer, bool aValue)
{
	myCollisionLayer = aCollisionLayer;

	physx::PxFilterData filterData = myShape->getSimulationFilterData();

	if (aValue)
		filterData.word0 |= static_cast<int>(myCollisionLayer); // Set the bit at position flagIndex
	else
		filterData.word0 &= ~static_cast<int>(myCollisionLayer); // Clear the bit at position flagIndex

	auto cur_index = std::countr_zero(static_cast<unsigned>(myCollisionLayer));

	filterData.word1 = PhysicsEngine::layerCollision[cur_index];

	myShape->setSimulationFilterData(filterData);
	myShape->setQueryFilterData(filterData);
}

Collider::~Collider()
{
	MainSingleton::Get()->activeScene->myPhysicsScene.myPhysicsScene->removeActor(*myActor);

	myActor->release();
	myActor = nullptr;
}

//Collider::Collider(PxShape* aShape, float aStaticFriction, float aDynamicFriction, float aBounciness)
//{
//	myShape = aShape;
//	SetPhysicsMaterial(aStaticFriction, aDynamicFriction, aBounciness);

//	auto rb = gameObject->GetComponent<RigidBody>();

//	if (rb)
//	{
//		rb->GetActor()->attachShape(*myShape);
//	}
//}

//void Collider::SetPhysicsMaterial(const PhysicsMaterial& aPhysicsMaterial)
//{
//	myPhysicsMaterial = aPhysicsMaterial;

//	physx::PxMaterial* material = nullptr;

//	myShape->setMaterials(&material, 1);
//}

//void Collider::SetPhysicsMaterial(float aStaticFriction, float aDynamicFriction, float aBounciness)
//{
//	myPhysicsMaterial.staticFriction = aStaticFriction;
//	myPhysicsMaterial.dynamicFriction = aDynamicFriction;
//	myPhysicsMaterial.bounciness = aBounciness;
//}

void Collider::SetIsTrigger(bool isTrigger)
{
	myIsTrigger = isTrigger;

	//if (!myShape)
	//	return;

	if (myIsTrigger)
	{
		myShape->setFlags(physx::PxShapeFlag::eTRIGGER_SHAPE | physx::PxShapeFlag::eSCENE_QUERY_SHAPE);
	}
	else
	{
		myShape->setFlags(physx::PxShapeFlag::eSIMULATION_SHAPE | physx::PxShapeFlag::eSCENE_QUERY_SHAPE);
	}
}

physx::PxShape* Collider::GetShape() const
{
	return myShape;
}

void Collider::SetShape(physx::PxShape* aShape)
{
	myShape = aShape;

	if (myActor)
	{
		myActor->attachShape(*myShape);
	}
}

void Collider::SetPositionOffset(const CU::Vector3f& aPositionOffset)
{
	myPositionOffset = aPositionOffset;

	//if (!myShape)
	//	return;

	PxVec3 pxPosition(myPositionOffset);
	PxQuat pxRotation(myRotationOffset);

	PxTransform localTransform(pxPosition, pxRotation);

	myShape->setLocalPose(localTransform);
}

void Collider::SetRotationOffsetEuler(const CU::Vector3f& aRotationOffset)
{
	myRotationOffset = aRotationOffset.ToQuarternion();

	PxVec3 pxPosition(myPositionOffset);
	PxQuat pxRotation(myRotationOffset);

	PxTransform localTransform(pxPosition, pxRotation);

	myShape->setLocalPose(localTransform);
}

void Collider::SetRotationOffsetQuaternion(const CU::Quaternion& aRotationOffset)
{
	myRotationOffset = aRotationOffset;

	PxVec3 pxPosition(myPositionOffset);
	PxQuat pxRotation(myRotationOffset);

	PxTransform localTransform(pxPosition, pxRotation);

	myShape->setLocalPose(localTransform);
}

void Collider::UpdateTransform() const
{
	if (!gameObject->GetComponent<RigidBody>())
	{
		CommonUtilities::Vector3f position = gameObject->transform->position;
		CommonUtilities::Quaternion rotation = gameObject->transform->rotation.ToVector3().ToQuarternion();

		rotation.Normalize();

		PxTransform tmActor;
		tmActor.p = PxVec3(position);
		tmActor.q = PxQuat(rotation);

		myActor->setGlobalPose(tmActor);

		physx::PxTransform tmCollider;
		tmCollider.p = physx::PxVec3(myPositionOffset);
		tmCollider.q = physx::PxQuat(myRotationOffset);

		myShape->setLocalPose(tmCollider);
	}
	
}

void Collider::Update(float)
{
	/*if (MainSingleton::Get()->myDebugHud->IsFlagSet("Colliders"))
		DrawCollider();*/

		//if (gameObject->GetTransform().GetIsDirty())
		//{
		//}

	UpdateTransform();
}

void Collider::RenderImGUI()
{
	ImGui::Text("Collisionlayer");

	ImGui::SameLine();

	if (ImGui::Button(PhysicsEngine::GetLayerToName(myCollisionLayer)))
		ImGui::OpenPopup("MeshColliderLayer");

	if (ImGui::BeginPopup("MeshColliderLayer"))
	{
		std::vector<std::string> meshes;

		for (int i = 0; i < collisionLayerCount; i++)
		{
			meshes.push_back(PhysicsEngine::GetLayerToName(static_cast<CollisionLayer>(1 << i)));
		}

		ImGui::SeparatorText("Meshes");
		for (int i = 0; i < meshes.size(); i++)
		{
			if (ImGui::Selectable(meshes[i].c_str()))
			{
				myCollisionLayer = static_cast<CollisionLayer>(1 << i);
			}
		}

		ImGui::EndPopup();
	}
}

void Collider::Disable()
{
	myIsDisabled = true;
}

void Collider::Enable()
{
	myIsDisabled = false;
}

//void Collider::Pause()
//{
//	if (myState == ComponentState::Paused)
//		return;
//
//	if (!myShape)
//		return;
//
//	Component::Pause();
//
//	myShape->setFlags(static_cast<PxShapeFlags>(0));
//}
//
//void Collider::UnPause()
//{
//	if (!myShape)
//		return;
//
//	Component::UnPause();
//
//	myShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, myIsTrigger);
//	myShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !myIsTrigger);
//	myShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
//
//	//myShapeFlags = PxShapeFlags();
//}

void Collider::Start()
{
	auto rb = gameObject->GetComponent<RigidBody>();

	if (rb)
	{
		rb->GetActor()->attachShape(*myShape);
		myActor = rb->GetActor();
	}
	else
	{
		myActor = PhysicsEngine::Get().CreateRigidBody(true)->is<PxRigidStatic>();
		myActor->userData = gameObject;

		if (gameObject->GetParent())
		{
			myActor->userData = gameObject->GetParent();
		}

		myActor->attachShape(*myShape);

	}

	myShape->release();

	MainSingleton::Get()->activeScene->myPhysicsScene.myPhysicsScene->addActor(*myActor);
	SetCollisionLayer(myCollisionLayer);
}

void Collider::SetOffsets(const CU::Vector3f& aPositionOffset, const CU::Vector3f& aRotationOffset)
{
	myPositionOffset = aPositionOffset;
	myRotationOffset = aRotationOffset.ToQuarternion();

	//if (!myShape)
	//	return;

	PxVec3 pxPosition(myPositionOffset);
	PxQuat pxRotation(myRotationOffset);

	PxTransform localTransform(pxPosition, pxRotation);

	myShape->setLocalPose(localTransform);
}