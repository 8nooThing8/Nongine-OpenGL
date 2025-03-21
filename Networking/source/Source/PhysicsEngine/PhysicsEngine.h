#pragma once

#include <physx\PxPhysics.h>
#include <physx\PxPhysicsAPI.h>

#include "physx\PxSimulationEventCallback.h"

#include <Vector\Vector.hpp>

#include <iostream>

#include "PhysicsScene.h"

#include "Primitive\Ray.hpp"

#include <atomic>

#include <vector>

using namespace physx;

class GameObject;

constexpr int collisionLayerCount = 4;

enum class CollisionLayer : physx::PxU32
{
	Default = 1 << 0,
	Player = 1 << 1,
	Trigger = 1 << 3,
	IgnoreRaycast = 1 << 4,
	All = (1 << collisionLayerCount) - 1
};

class CollisionCallback : public PxSimulationEventCallback
{
public:
	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;
	void onTrigger(PxTriggerPair* pairs, PxU32 count) override;

	void onConstraintBreak(PxConstraintInfo*, PxU32) override {}
	void onWake(PxActor**, PxU32) override {}
	void onSleep(PxActor**, PxU32) override {}
	void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) override {}
};

class MyErrorCallback : public physx::PxErrorCallback {
public:
	void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) PX_OVERRIDE
	{
		// Handle the error, log it, or assert based on severity
		std::cerr << "PhysX error (" << code << "): " << message << " in file " << file << " at line " << line << std::endl;
	}
};

struct Hit
{
	CommonUtilities::Vector3<float> position;	// World-space hit position
	CommonUtilities::Vector3<float> normal;	// World-space hit normal
	float distanceToHit;	// Distance from the origin to the hitposition

	GameObject* hitObject;	// Gameobject that the ray hit
};

struct Hits
{
	unsigned nbHits;
	std::vector<Hit> hits;
};

class CustomInputData : physx::PxInputData
{
private:
	std::vector<uint8_t> data;
	uint32_t currentPos = 0;
public:
	CustomInputData(const std::vector<uint8_t>& inputData);

	virtual uint32_t getLength() const override;

	virtual void seek(uint32_t offset) override;

	virtual uint32_t tell() const override;

	virtual uint32_t read(void* dest, uint32_t count) override;
};

namespace TGA
{
	namespace FBX
	{
		struct Mesh;
	}
}

class PhysicsEngine
{
public:
	static int layerCollision[collisionLayerCount];

	static inline const char* GetLayerToName(CollisionLayer aCollisonlayer)
	{
		switch (aCollisonlayer)
		{
		case CollisionLayer::Default:
			return "Default";
			break;
		case CollisionLayer::Player:
			return "Player";
			break;
		case CollisionLayer::Trigger:
			return "Trigger";
			break;
		case CollisionLayer::IgnoreRaycast:
			return "IgnoreRaycast";
			break;
		case CollisionLayer::All:
			return "All";
			break;
		default:
			return "None";
			break;
		}
	}

	static void LoadLayerEditFromJSON();

	static PhysicsEngine& Get();

	// Create a rigidbody with default settings
	physx::PxRigidActor* CreateRigidBody(bool isStatic) const;

	// Create Colliders
	/**
	 *Create Mesh collider
	 *
	 *@param [in]aMesh The mesh the meshcollider will use to determine collision
	 *
	 */
	physx::PxShape* CreateMeshCollider(TGA::FBX::Mesh* aMesh) const;
	/**
	 *Create Box collider
	 *
	 *@param [in]aHalfExtent The size of the collider to be created
	 *
	 */
	physx::PxShape* CreateBoxCollider(CommonUtilities::Vector3<float> aHalfExtent) const;
	/**
	*Create Sphere collider
	*
	*@param [in]aSize The size of the collider to be created
	*
	*/
	physx::PxShape* CreateSphereCollider(float aRadius) const;

	/**
	*Create Capsule collider
	*
	*@param [in]aSize The size of the collider to be created
	*
	*/
	physx::PxShape* CreateCapsuleCollider(float aRadius, float aHalfExtents) const;


	// Static object creation functions


	bool Initialize();

	void NewScene(PhysicsScene& aScene);

	void Update();

	static bool RayCast(CommonUtilities::Ray<float>& aRay, Hit& outHits, float maxDistance = 10000000.f, physx::PxU32 aLayers = static_cast<physx::PxU32>(CollisionLayer::All), bool aDebugDrawRay = false);
	static bool RayCasts(CommonUtilities::Ray<float>& aRay, Hits& outHits, float maxDistance = 10000000.f, physx::PxU32 aLayers = static_cast<physx::PxU32>(CollisionLayer::All), bool aDebugDrawRay = false);
	//static bool RayCastScreenToWorld(physx::PxU32 aLayers, Hits& outHits, bool aDebugDrawRay = false);
	static bool OverlapSphere(physx::PxU32 aLayers, Hits& outHits, float radius, CommonUtilities::Vector3<float> aOffset);
	static bool OverlapBox(physx::PxU32 aLayers, Hits& outHits, const CommonUtilities::Vector3<float>& aSize, const CommonUtilities::Vector3<float>& aRotation, const CommonUtilities::Vector3<float>& aOffset);

	physx::PxPhysics* myPhysics = nullptr;

	//physx::PxCudaContextManager* myCudaManager;

	CollisionCallback* collision;
	physx::PxMaterial* myDefaultPhysicsMaterial = nullptr;
	physx::PxPvd* myPvd = nullptr;

	std::atomic_bool physicsSimulationDone;

	// Not working
	physx::PxFEMCloth* NewClothObject(CommonUtilities::Vector3<float> aPosition, CommonUtilities::Vector3<float> aSize, physx::PxQuat aRotationQuertenion = physx::PxQuat(0, 0, 0, 1));
	physx::PxConstraint* NewJoint(PxRigidActor* rigidActor1, PxRigidActor* rigidActor2, PxConstraintConnector& connector, const PxConstraintShaderTable& shaders, PxU32 dataSize);
private:

	PhysicsEngine() = default;
	~PhysicsEngine() = default;

	physx::PxDefaultAllocator myDefaultAllocator;
	MyErrorCallback myDefaultErrorCallback;
	physx::PxTolerancesScale myTolerancesScale;

	physx::PxFoundation* myFoundation = nullptr;
	physx::PxDefaultCpuDispatcher* myDispatcher = nullptr;

	PxScene* tstScene;

	int myPrevSceneIndex = 0;
};