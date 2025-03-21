#include "../Engine/Engine.pch.h"

#include "../Engine/GameEngine/MainSingleton.h"
#include "../Engine/GameEngine/SceneManagerInternal.h"
#include "../Engine/GameEngine/Components/GameObject.h"

#include "../Physics/RigidBody.h"
#include "physx\PxSimulationEventCallback.h"

#include "../PhysicsEngine/PhysicsEngine.h"

#include <InputHandler.h>

#ifdef _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(RigidBodyLog, "RigidBody", Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(RigidBodyLog, "RigidBody", Error);
#endif // _DEBUG

DEFINE_LOG_CATEGORY(RigidBodyLog);

RigidBody::RigidBody(bool /*isStatic*/)
{
	Component();

	
}

//void TreRaderKod::RigidBody::SetIsTrigger(bool isTrigger)
//{
//	PxRigidActor* rActor = myBody->is<physx::PxRigidActor>();
//
//	PxU32 numShapes = rActor->getNbShapes();
//
//	PxShape** shapes = new PxShape * [numShapes];
//
//	rActor->getShapes(shapes, numShapes);
//	PxShapeFlags flaga = shapes[0]->getFlags();
//
//	for (PxU32 i = 0; i < numShapes; i++)
//	{
//		PxShape* shape = shapes[i];
//
//		shape->setFlags((!isTrigger ? physx::PxShapeFlag::eSIMULATION_SHAPE : physx::PxShapeFlag::eTRIGGER_SHAPE) | physx::PxShapeFlag::eSCENE_QUERY_SHAPE);
//
//		//shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, isTrigger);
//		//shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !isTrigger);
//	}
//
//	delete[] shapes;
//}

RigidBody::~RigidBody()
{
	myBody->userData = nullptr;

	MainSingleton::Get()->activeScene->myPhysicsScene.myPhysicsScene->removeActor(*myBody);

	myBody->release();
	myBody = nullptr;
}

void RigidBody::SetDensity(float aDensity) const
{
	auto rigidDynamic = myBody->is<PxRigidDynamic>();

	if (!rigidDynamic)
	{
		LOG(RigidBodyLog, Warning, "You can not set mass and inertia for a static rigid body");
		return;
	}

	physx::PxRigidBodyExt::updateMassAndInertia(*rigidDynamic, aDensity);
}

void RigidBody::Start()
{
	myBody = PhysicsEngine::Get().CreateRigidBody(false);
	myBody->userData = gameObject;
}

void RigidBody::Update(float aDeltaTime)
{
	aDeltaTime;

	/*
	if (CU::InputManager::GetInstance().IsKeyPressed(CommonUtilities::Keys::E))
	{
		SetIsStatic(!myIsStatic);
	}
	if (CU::InputManager::GetInstance().IsKeyPressed(CommonUtilities::Keys::R))
	{
		if (myBody->is<PxRigidStatic>())
		{
			SetIsTrigger(true);
		}
	}*/

	//if (InputHandler::GetInstance()->GetKeyDown(keycode::SPACE))
	//{
	//	SetVelocity(CommonUtilities::Vector3f(0, 100, 0));
	//}

	if (myBody)
	{
		if (myIsStatic)
		{
			UpdateStatic();
		}
		else
		{
			UpdateDynamic();
		}
	}
}

//void TreRaderKod::RigidBody::DrawCollider()
//{
//	if (myColliderType == ColliderType::Box)
//	{
//		DrawBoxCollider();
//	}
//	else if (myColliderType == ColliderType::Capsule)
//	{
//		DrawBoxCollider();
//	}
//	else
//	{
//		DrawSphereCollider();
//	}
//}

void RigidBody::UpdateTransform() const
{
	// Assuming T is your specific actor type, e.g., PxRigidDynamic or PxRigidStatic
	physx::PxTransform tm = myBody->getGlobalPose();

	CommonUtilities::Vector3<float> position = gameObject->transform->position;
	CommonUtilities::Quaternion rotation = gameObject->transform->rotation.ToEuler().ToQuarternion();

	tm.p = physx::PxVec3(position); // Set the position
	tm.q = physx::PxQuat(rotation); // Set the rotation quaternion

	myBody->setGlobalPose(tm); // Update the global pose
}

void RigidBody::UpdateStatic()
{
	UpdateTransform();
}

void RigidBody::UpdateDynamic()
{
	//UpdateTransform();
	//if (gameObject->GetTransform().GetIsDirty())
	//	UpdateTransform();
	//else
	//{
	//	PxRigidDynamic* myDynamicBody = myBody->is<PxRigidDynamic>();

	//	if (CU::InputManager::GetInstance().IsKeyDown(CommonUtilities::Keys::UP))
	//	{
	//		myDynamicBody->setLinearVelocity(PxVec3(myDynamicBody->getLinearVelocity().x, myDynamicBody->getLinearVelocity().y, 100));
	//	}
	//	if (CU::InputManager::GetInstance().IsKeyDown(CommonUtilities::Keys::DOWN))
	//	{
	//		myDynamicBody->setLinearVelocity(PxVec3(myDynamicBody->getLinearVelocity().x, myDynamicBody->getLinearVelocity().y, -100));
	//	}

	//	if (CU::InputManager::GetInstance().IsKeyDown(CommonUtilities::Keys::RIGHT))
	//	{
	//		myDynamicBody->setLinearVelocity(PxVec3(100, myDynamicBody->getLinearVelocity().y, myDynamicBody->getLinearVelocity().z));
	//	}
	//	if (CU::InputManager::GetInstance().IsKeyDown(CommonUtilities::Keys::LEFT))
	//	{
	//		myDynamicBody->setLinearVelocity(PxVec3(-100, myDynamicBody->getLinearVelocity().y, myDynamicBody->getLinearVelocity().z));
	//	}

	//	if (CU::InputManager::GetInstance().IsKeyDown(CommonUtilities::Keys::Q))
	//	{
	//		myDynamicBody->setLinearVelocity(PxVec3(myDynamicBody->getLinearVelocity().x, 250.f, myDynamicBody->getLinearVelocity().z));

	//		//SetIsStatic(true);
	//	}

	//	if (CU::InputManager::GetInstance().IsKeyPressed(CommonUtilities::Keys::Z))
	//	{
	//		//SetPhysicsMaterial(0, 0, 20);
	//	}

		/*CommonUtilities::Vector3<float> offsetRightAmount = myOffset.x * gameObject->GetTransform().LocalRight();
		CommonUtilities::Vector3<float> offsetUpAmount = myOffset.y * gameObject->GetTransform().LocalUp();
		CommonUtilities::Vector3<float> offsetForwardAmount = myOffset.z * gameObject->GetTransform().LocalForward();
		*/
		gameObject->transform->position = CommonUtilities::Vector3f(myBody->getGlobalPose().p);

		//physx::PxQuat rotation = myBody->getGlobalPose().q;

		//gameObject->transform->rotation = CommonUtilities::Vector4f(rotation.x, rotation.y, rotation.z, rotation.w).ToEuler();
	//}
}

void RigidBody::SetIsStatic(bool aIsStatic)
{
	if (myBody->is<PxRigidStatic>())
	{
		LOG(RigidBodyLog, Error, "A static collider can not be set to non static");
		return;
	}

	PxRigidDynamic* myDynamicBody = myBody->is<PxRigidDynamic>();

	if (aIsStatic)
		myDynamicBody->putToSleep();
	else
		myDynamicBody->wakeUp();

	myIsStatic = aIsStatic;
}

void RigidBody::SetVelocity(const CU::Vector3<float>& aVelocity)
{
	PxRigidDynamic* rbDynamic = myBody->is<PxRigidDynamic>();

	if (rbDynamic)
		rbDynamic->setLinearVelocity(physx::PxVec3(aVelocity));

	LOG(RigidBodyLog, Error, "A static RigidBody's linear velocity can not be set");

}

CU::Vector3<float> RigidBody::GetVelocity() const
{
	PxRigidDynamic* rbDynamic = myBody->is<PxRigidDynamic>();

	if (rbDynamic)
		return rbDynamic->getLinearVelocity();

	return CU::Vector3<float>(0, 0, 0);
}

float RigidBody::GetMagnitude()
{
	return GetVelocity().Length();
}

void RigidBody::SetAngularVelocity(const CU::Vector3<float>& aVelocity)
{
	PxRigidDynamic* rbDynamic = myBody->is<PxRigidDynamic>();

	if (rbDynamic)
		rbDynamic->setAngularVelocity(physx::PxVec3(aVelocity));
	else
	{
		LOG(RigidBodyLog, Error, "A static RigidBody's angular velocity can not be set");
		return;
	}
}

const CU::Vector3<float> RigidBody::GetAngularVelocity()
{
	PxRigidDynamic* rbDynamic = myBody->is<PxRigidDynamic>();

	if (rbDynamic)
		return rbDynamic->getAngularVelocity();

	return CU::Vector3<float>(0, 0, 0);
}

void RigidBody::Sleep() const
{
	myBody->is<physx::PxRigidDynamic>()->putToSleep();
}

void RigidBody::Wake() const
{
	myBody->is<physx::PxRigidDynamic>()->wakeUp();
}

//void TreRaderKod::RigidBody::SetMaterials(physx::PxMaterial* aMaterial) const
//{
//	PxRigidActor* rActor = myBody->is<physx::PxRigidActor>();
//
//	PxU32 numShapes = rActor->getNbShapes();
//
//	PxShape** shapes = new PxShape * [numShapes];
//
//	rActor->getShapes(shapes, numShapes);
//	PxShapeFlags flaga = shapes[0]->getFlags();
//
//	for (PxU32 i = 0; i < numShapes; i++)
//	{
//		PxShape* shape = shapes[i];
//
//		shape->setMaterials(&aMaterial, 1);
//	}
//
//	delete[] shapes;
//}

//void TreRaderKod::RigidBody::AddCollideWith(const CollisionLayer& aLayer)
//{
//	myCollideWithLayer |= static_cast<int>(aLayer);
//}
//
//void TreRaderKod::RigidBody::AddmyCollisionLayer(const CollisionLayer& aLayer)
//{
//	myThisCollisionLayer |= static_cast<int>(aLayer);
//}

PxRigidActor* RigidBody::GetActor() const
{
	return myBody;
}

//void RigidBody::OnCollision(GameObject*& aOther)
//{
//	UNREFERENCED_PARAMETER(aOther);
//
//	//std::cout << "C";
//}
//
//void RigidBody::OnCollisionEnter(GameObject*& aOther)
//{
//	UNREFERENCED_PARAMETER(aOther);
//
//	//std::cout << "Colision enter" << std::endl;
//}
//
//void RigidBody::OnCollisionExit(GameObject*& aOther)
//{
//	UNREFERENCED_PARAMETER(aOther);
//	//std::cout << "Colision exit" << std::endl;
//}
