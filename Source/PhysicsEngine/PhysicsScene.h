#pragma once

namespace physx
{
	class PxScene;
	class PxControllerManager;
}

struct PhysicsScene
{
	physx::PxScene* myPhysicsScene;
	physx::PxControllerManager* myCharacterControllerManager;
};