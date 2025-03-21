#include "../../../Engine.pch.h"
#include "CharacterController.h"

#include "../Engine/GameEngine/MainSingleton.h"
#include "../Engine/GameEngine/SceneManagerInternal.h"

#include "../Engine/GameEngine/Components/GameObject.h"

CharacterController::CharacterController(float aHalfHeight, float aRadius, CollisionLayer aCollisionLayer, const CommonUtilities::Vector3f& aRotOffset) :
	myCollisionLayer(aCollisionLayer), myRadius(aRadius), myHalfHeight(aHalfHeight), rotOffset(aRotOffset)
{
	Component();

	//MainSingleton::Get()->myDebugHud->AddCheckBox("Colliders", DebugCategory::Level);
	//MainSingleton::Get()->myDebugHud->AddCheckBox("Character colliders", DebugCategory::Level);
}

CharacterController::CharacterController(float aHalfHeight, float aRadius) : myRadius(aRadius), myHalfHeight(aHalfHeight)
{
	Component();

	//MainSingleton::Get()->myDebugHud->AddCheckBox("Colliders", DebugCategory::Level);
	//MainSingleton::Get()->myDebugHud->AddCheckBox("Character colliders", DebugCategory::Level);
}

CharacterController::~CharacterController()
{
	std::cout << "delete";

	auto& controller = GetController();
	if (controller)
	{
		auto actor = controller->getActor();
		if (actor != nullptr)
		{
			actor->userData = nullptr;
		}

		MainSingleton::Get()->activeScene->myPhysicsScene.myPhysicsScene->removeActor(*actor);

		controller->release();
		controller = nullptr;
	}

}

void CharacterController::Start()
{
	if (myController)
		return;

	physx::PxCapsuleControllerDesc desc = {};

	desc.height = myHalfHeight * 2.f;                 // Height of the character
	desc.radius = myRadius;                 // Radius (for a capsule controller)
	desc.position = PxExtendedVec3(0.f, 0.f, 0.f); // Start position
	desc.material = PhysicsEngine::Get().myPhysics->createMaterial(0.1f, 0.1f, 0.f); // Static friction, dynamic friction, restitution
	desc.stepOffset = 0.5f;             // Maximum step height to be climbed
	desc.slopeLimit = cos(PxPi / 4.0f); // Maximum slope angle the character can walk up
	desc.upDirection = PxVec3(0, 1, 0); // Direction of "up"
	desc.contactOffset = 0.1f;

	myControllerProperties = desc;

	myFilterData.word0 = static_cast<PxU32>(myCollisionLayer);

	auto cur_index = std::countr_zero(static_cast<unsigned>(myCollisionLayer));
	myFilterData.word1 = PhysicsEngine::layerCollision[cur_index];

	myFilters = PxControllerFilters(&myFilterData);
	myFilters.mCCTFilterCallback = new ControllerCallback;

	myController = MainSingleton::Get()->activeScene->myPhysicsScene.myCharacterControllerManager->createController(myControllerProperties);

	//PxShape* shapes[1] = {nullptr};
	//myController->getActor()->getShapes(shapes, 1);

	//PxShape* shape = shapes[0];

	//shape->setQueryFilterData(*myFilters.mFilterData);

	CommonUtilities::Vector3f position = gameObject->transform->position;
	myController->setFootPosition(PxExtendedVec3(position.x, position.y, position.z));

	auto controller = GetController();
	if (controller != nullptr)
	{
		auto actor = controller->getActor();
		if (actor != nullptr)
		{
			actor->userData = gameObject;
		}
		else
		{
			std::cout << "Actor was null \n";
		}
	}
	else
	{
		std::cout << "Controller was null \n";
	}

	myVelocity = CommonUtilities::Vector3f{ 0.f,0.f,0.f };
}

void CharacterController::Update(float aDeltaTime)
{

	lastIsStandingOnGround = IsStandingOnGround();

	CommonUtilities::Vector3f finalVelocityCU;

	if (myUseGravity)
		myVelocity += CommonUtilities::Vector3f(MainSingleton::Get()->activeScene->myPhysicsScene.myPhysicsScene->getGravity()) * myGravityFactor * aDeltaTime;

	finalVelocityCU = myVelocity * aDeltaTime;
	PxVec3 finalVelocity(finalVelocityCU);

	myCurrentCollisionFlags = myController->move(finalVelocity, 0.001f, aDeltaTime, myFilters);

	CommonUtilities::Vector3f CUVec3(myController->getFootPosition());

	gameObject->transform->position = (CUVec3 + offset);


	if (!lastIsStandingOnGround && IsStandingOnGround())
	{
		myVelocity.y = 0;
	}
}

void CharacterController::SetCollisionLayer(CollisionLayer aLayer)
{
	myCollisionLayer = aLayer;
}

void CharacterController::SetVelocity(const CommonUtilities::Vector3f& aVelocity)
{
	myVelocity = aVelocity;
}

void CharacterController::AddVelocity(const CommonUtilities::Vector3f& aVelocity)
{
	myVelocity += aVelocity;
}

CommonUtilities::Vector3f& CharacterController::GetVelocity()
{
	return myVelocity;
}

void CharacterController::TurnOffGravity()
{
	myUseGravity = false;
}

void CharacterController::SetUseGravity(bool aUseGravity)
{
	myUseGravity = aUseGravity;
}

void CharacterController::SetFootPosition(const CommonUtilities::Vector3f& aFootPosition)
{
	if (myController)
	{
		myController->setFootPosition(PxExtendedVec3(aFootPosition.x, aFootPosition.y, aFootPosition.z));
	}
}

void CharacterController::SetPosition(const CommonUtilities::Vector3f& aFootPosition)
{
	if (myController)
	{
		myController->setPosition(PxExtendedVec3(aFootPosition.x, aFootPosition.y, aFootPosition.z));
	}
}

void CharacterController::SetStepHeight(float aStepHeight) const
{
	myController->setStepOffset(aStepHeight);
}

void CharacterController::SetGravityFactor(float aFactor)
{
	myGravityFactor = aFactor;
}

physx::PxController*& CharacterController::GetController()
{
	return myController;
}

float CharacterController::GetRadius() const
{
	return myRadius;
}

bool CharacterController::IsStandingOnGround() const
{
	return myCurrentCollisionFlags.isSet(PxControllerCollisionFlag::eCOLLISION_DOWN);
}

void CharacterController::SetRadius(float aRadius)
{
	myRadius = aRadius;
}

void CharacterController::SetHalfHeigt(float aHalfHeight)
{
	myHalfHeight = aHalfHeight;
}
