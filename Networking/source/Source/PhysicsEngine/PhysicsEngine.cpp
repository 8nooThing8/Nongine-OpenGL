#include "PhysicsEngine.pch.h"
#include "PhysicsEngine.h"
#include "Logger\Logger.h"

#include "rapidjson-master\include\rapidjson\rapidjson.h"
#include "rapidjson-master\include\rapidjson\Document.h"

#include "../Engine/GameEngine/MainSingleton.h"
#include "../Engine/GameEngine/SceneManagerInternal.h"

#include "../Engine/GameEngine/Components/GameObject.h"

#include "physx\PxSimulationEventCallback.h"

#include <InputHandler.h>

#include <physx/cooking/PxCooking.h>

#include "TgaFbxStructs.h"

#include "Timer.h"

#undef min

#ifdef _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(PhysicsEgnineLog, "PhysicsEgnine", Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(PhysicsEgnineLog, "PhysicsEgnine", Error);
#endif // _DEBUG

DEFINE_LOG_CATEGORY(PhysicsEgnineLog);

int PhysicsEngine::layerCollision[collisionLayerCount] = {}; // Rest will be 0

void PhysicsEngine::LoadLayerEditFromJSON()
{
	std::string filePath = "Settings/Collisionlayers.json";

	std::ifstream datastream(filePath, std::ios::in);
	assert(datastream);

	std::stringstream stream;

	char c;
	while (datastream.get(c))
	{
		stream << c;
	}

	rapidjson::Document data;
	data.Parse(stream.str().c_str());

	auto allocator = data.GetAllocator();

	auto layers = data["Layer"].GetArray();

	for (unsigned i = 0; i < layers.Size(); i++)
	{
		layerCollision[i] = layers[i].GetInt();
	}
}

PhysicsEngine& PhysicsEngine::Get()
{
	static PhysicsEngine myInstance;
	return myInstance;
}

physx::PxRigidActor* PhysicsEngine::CreateRigidBody(bool isStatic) const
{
	PxRigidActor* body;

	if (!isStatic)
	{
		body = myPhysics->createRigidDynamic(physx::PxTransform(PxVec3(0, 0, 0)));
		MainSingleton::Get()->activeScene->myPhysicsScene.myPhysicsScene->addActor(*body);
	}
	else
	{
		body = myPhysics->createRigidStatic(physx::PxTransform(PxVec3(0, 0, 0)));
	}

	return body;
}

physx::PxShape* PhysicsEngine::CreateMeshCollider(TGA::FBX::Mesh* aMesh) const
{
	std::vector<TGA::FBX::Vertex> aVertecies;
	std::vector<unsigned> aIndecies;

	size_t totalVertCount = 0;
	size_t totalindexCount = 0;

	for (size_t i = 0; i < aMesh->Elements.size(); i++)
	{
		totalVertCount += aMesh->Elements[i].Vertices.size();
		totalindexCount += aMesh->Elements[i].Indices.size();
	}

	aVertecies.resize(totalVertCount);
	aIndecies.resize(totalindexCount);

	size_t vertexIndent = 0;
	size_t indexIndent = 0;

	for (int i = 0; i < aMesh->Elements.size(); i++)
	{
		const auto& element = aMesh->Elements[i];

		std::memcpy(aVertecies.data() + vertexIndent, element.Vertices.data(), element.Vertices.size() * sizeof(TGA::FBX::Vertex));

		for (size_t j = 0; j < element.Indices.size(); j++)
		{
			aIndecies[indexIndent + j] = element.Indices[j] + static_cast<unsigned>(vertexIndent);
		}

		vertexIndent += element.Vertices.size();
		indexIndent += element.Indices.size();
	}


	PxCookingParams params = PxTolerancesScale();
	params.midphaseDesc = PxMidphaseDesc();

	PxTriangleMeshDesc desc;

	std::vector<PxVec3> positions(aVertecies.size());

	for (size_t i = 0; i < aVertecies.size(); i++)
	{
		positions[i] = PxVec3(aVertecies[i].Position[0], aVertecies[i].Position[1], aVertecies[i].Position[2]);
	}

	desc.points.count = static_cast<PxU32>(positions.size());
	desc.points.stride = sizeof(PxVec3);
	desc.points.data = positions.data();

	desc.triangles.count = static_cast<PxU32>(aIndecies.size() / 3);
	desc.triangles.stride = static_cast<PxU32>(3 * sizeof(unsigned));
	desc.triangles.data = aIndecies.data();

	PxDefaultMemoryOutputStream buf;
	bool status = PxCookTriangleMesh(params, desc, buf, nullptr);

	if (!status)
	{
		std::cout << "you failed" << std::endl;

		__debugbreak();
	}

	PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	PxTriangleMesh* triangleMesh = myPhysics->createTriangleMesh(input);

	physx::PxShape* shape = myPhysics->createShape(physx::PxTriangleMeshGeometry(triangleMesh), *myDefaultPhysicsMaterial, true);
	return shape;
}

physx::PxShape* PhysicsEngine::CreateBoxCollider(CommonUtilities::Vector3<float> aHalfExtent) const
{
	physx::PxShape* shape = myPhysics->createShape(physx::PxBoxGeometry(physx::PxVec3(aHalfExtent)), *myDefaultPhysicsMaterial, true);
	return shape;
}

physx::PxShape* PhysicsEngine::CreateSphereCollider(float aRadius) const
{
	physx::PxShape* shape = myPhysics->createShape(physx::PxSphereGeometry(aRadius), *myDefaultPhysicsMaterial, true);
	return shape;
}

physx::PxShape* PhysicsEngine::CreateCapsuleCollider(float aRadius, float aHalfHeight) const
{
	physx::PxShape* shape = myPhysics->createShape(physx::PxCapsuleGeometry(aRadius, aHalfHeight), *myDefaultPhysicsMaterial, true);
	return shape;
}

physx::PxFEMCloth* PhysicsEngine::NewClothObject(CommonUtilities::Vector3<float> /*aPosition*/, CommonUtilities::Vector3<float> /*aSize*/, physx::PxQuat /*aRotationQuertenion*/)
{
	physx::PxFEMClothMaterial* mat = myPhysics->createFEMClothMaterial(1.f, 1.f, 1.f);

	mat;

	//physx::PxFEMCloth* body = myPhysics->createFEMCloth(*myCudaManager);

	return nullptr;
}

physx::PxConstraint* PhysicsEngine::NewJoint(PxRigidActor* rigidActor1, PxRigidActor* rigidActor2, PxConstraintConnector& connector, const PxConstraintShaderTable& shaders, PxU32 dataSize)
{
	physx::PxConstraint* constaint = myPhysics->createConstraint(rigidActor1, rigidActor2, connector, shaders, dataSize);



	return constaint;
}

PxFilterFlags CustomFilterShader(
	PxFilterObjectAttributes /*attributes0*/, PxFilterData /*filterData0*/,
	PxFilterObjectAttributes /*attributes1*/, PxFilterData /*filterData1*/,
	PxPairFlags& pairFlags, const void* /*constantBlock*/, PxU32 /*constantBlockSize*/)
{
	pairFlags = PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_LOST | PxPairFlag::eNOTIFY_TOUCH_CCD;

	return PxFilterFlag::eDEFAULT;
}

bool PhysicsEngine::Initialize()
{
	LoadLayerEditFromJSON();

	myFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, myDefaultAllocator, myDefaultErrorCallback);
	if (!myFoundation)
	{
		throw("PxCreateFoundation failed!");
	}

	myPvd = PxCreatePvd(*myFoundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	myPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
	myTolerancesScale.length = 100;
	myTolerancesScale.speed = 30000;
	myPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *myFoundation, myTolerancesScale, true, myPvd);

	collision = new CollisionCallback();

	//physx::PxCudaContextManagerDesc desc = {};

	//desc.graphicsDevice = GraphicsEngine::GetRHI()->GetDevice().Get();
	//desc.appGUID = "NVIDIA PhysX System Software 9.23.1019";

	//myCudaManager = PxCreateCudaContextManager(*myFoundation, desc, nullptr);

	myDefaultPhysicsMaterial = myPhysics->createMaterial(0.f, 0.f, 0.f);

	return true;
}

void PhysicsEngine::NewScene(PhysicsScene& aScene)
{
	physx::PxSceneDesc sceneDesc(myPhysics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -3000.f, 0.0f);
	myDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = myDispatcher;
	sceneDesc.filterShader = CustomFilterShader;

	sceneDesc.flags = PxSceneFlag::eENABLE_PCM;

	aScene.myPhysicsScene = myPhysics->createScene(sceneDesc);

	aScene.myCharacterControllerManager = PxCreateControllerManager(*aScene.myPhysicsScene, false);

	aScene.myPhysicsScene->setSimulationEventCallback(collision);
}

void PhysicsEngine::Update()
{
	const float deltaTime = CommonUtilities::Time::Get().GetDeltaTime();

	MainSingleton::Get()->activeScene->myPhysicsScene.myPhysicsScene->simulate(deltaTime);
	MainSingleton::Get()->activeScene->myPhysicsScene.myPhysicsScene->fetchResults(true);
}

bool PhysicsEngine::RayCast(CommonUtilities::Ray<float>& aRay, Hit& outHit, float maxDistance, physx::PxU32 aLayers, bool /*debugDrawRay*/)
{
	if (aRay.GetDirection().Length() == 0.f)
		return false;

	physx::PxVec3 PXorigin(aRay.GetOrigin());
	physx::PxVec3 PXdirection(aRay.GetDirection());
	//	debugDrawRay;
	//#ifdef _DEBUG
	//	if (debugDrawRay)
	//	{
	//		auto& engine = GraphicsEngine::Get();
	//		CU::Vector4f colour = { 0.f, 1.f, 0.f, 1.f }; // Green colour for the lines
	//
	//		LinePrimitive line;
	//
	//		CU::Vector3f originCU = CU::Vector3f(PXorigin);
	//		CU::Vector3f directionCU = CU::Vector3f(PXorigin + PXdirection * maxDistance);
	//
	//		line.myStartPoint = { originCU.ToVector4(1.f), colour };
	//		line.myEndPoint = { directionCU.ToVector4(1.f), colour };
	//
	//		engine.AddLine(line);
	//	}
	//#endif // DEBUG

	physx::PxQueryFilterData layersToCollideWith = physx::PxQueryFilterData();

	layersToCollideWith.flags.raise(PxQueryFlag::eDYNAMIC);
	layersToCollideWith.flags.raise(PxQueryFlag::eSTATIC);

	layersToCollideWith.data.word0 = aLayers;

	PxRaycastBuffer rayBuffer = PxRaycastBuffer();

	bool status = MainSingleton::Get()->activeScene->myPhysicsScene.myPhysicsScene->raycast(PXorigin, PXdirection, maxDistance, rayBuffer, PxHitFlag::eANY_HIT | PxHitFlag::eDEFAULT, layersToCollideWith);

	if (status)
	{
		auto& pxHit = rayBuffer.getAnyHit(0);

		outHit.hitObject = static_cast<GameObject*>(pxHit.actor->userData);
		outHit.normal = pxHit.normal;
		outHit.position = pxHit.position;
		outHit.distanceToHit = pxHit.distance;
	}

	return status;
}

bool PhysicsEngine::RayCasts(CommonUtilities::Ray<float>& aRay, Hits& outHits, float maxDistance, physx::PxU32 aLayers, bool /*debugDrawRay*/)
{
	if (aRay.GetDirection().Length() == 0.f)
		return false;

	physx::PxVec3 PXorigin(aRay.GetOrigin());
	physx::PxVec3 PXdirection(aRay.GetDirection());

	//debugDrawRay;

//#ifdef _DEBUG
//	if (debugDrawRay)
//	{
//		auto& engine = GraphicsEngine::Get();
//		CU::Vector4f colour = { 0.f, 1.f, 0.f, 1.f }; // Green colour for the lines
//
//		LinePrimitive line;
//
//		CU::Vector3f originCU = CU::Vector3f(PXorigin);
//		CU::Vector3f directionCU = CU::Vector3f(PXorigin + PXdirection * maxDistance);
//
//		line.myStartPoint = { originCU.ToVector4(1.f), colour };
//		line.myEndPoint = { directionCU.ToVector4(1.f), colour };
//
//		engine.AddLine(line);
//	}
//#endif // DEBUG

	physx::PxQueryFilterData layersToCollideWith = physx::PxQueryFilterData();

	layersToCollideWith.flags.raise(PxQueryFlag::eDYNAMIC);
	layersToCollideWith.flags.raise(PxQueryFlag::eSTATIC);

	layersToCollideWith.data.word0 = aLayers;

	PxRaycastHit hitBuffer[256]; // Define a sufficiently large buffer
	PxRaycastBuffer rayBuffer = PxRaycastBuffer(hitBuffer, 256);

	bool status = MainSingleton::Get()->activeScene->myPhysicsScene.myPhysicsScene->raycast(PXorigin, PXdirection, maxDistance, rayBuffer, PxHitFlag::eMESH_MULTIPLE | PxHitFlag::eDEFAULT, layersToCollideWith);

	outHits.nbHits = rayBuffer.getNbAnyHits();

	for (int i = 0; i < static_cast<int>(rayBuffer.getNbAnyHits()); i++)
	{
		auto& pxHit = rayBuffer.getAnyHit(i);

		Hit rayHit;

		rayHit.hitObject = static_cast<GameObject*>(pxHit.actor->userData);
		rayHit.normal = pxHit.normal;
		rayHit.position = pxHit.position;
		rayHit.distanceToHit = pxHit.distance;

		outHits.hits.emplace_back(rayHit);
	}

	return status;
}

// Casts a ray from mouse position to world
//bool PhysicsEngine::RayCastScreenToWorld(physx::PxU32 aLayers, Hits& outHits, bool debugDrawRay)
//{
//	auto mousePosition = CU::InputManager::GetInstance().GetMousePosition();
//	CU::Vector2f mousePointF = CU::Vector2f(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));
//	auto& camera = MainSingleton::Get()->mySceneManager->GetCamera()->GetCameraData();
//	CU::Ray<float> ray = CU::Ray<float>(camera.CameraPointToRay(mousePointF));
//
//	physx::PxVec3 PXorigin(ray.GetOrigin());
//	physx::PxVec3 PXdirection(ray.GetDirection());
//	physx::PxReal PXmaxDistance(10000000.f);
//
//	debugDrawRay;
//
//#ifdef _DEBUG
//	if (debugDrawRay)
//	{
//		auto& engine = GraphicsEngine::Get();
//		CU::Vector4f colour = { 0.f, 1.f, 0.f, 1.f }; // Green colour for the lines
//
//		LinePrimitive line;
//
//		CU::Vector3f originCU = CU::Vector3f(PXorigin);
//		CU::Vector3f directionCU = CU::Vector3f(PXorigin + PXdirection * PXmaxDistance);
//
//		line.myStartPoint = { originCU.ToVector4(1.f), colour };
//		line.myEndPoint = { directionCU.ToVector4(1.f), colour };
//
//		engine.AddLine(line);
//	}
//#endif // DEBUG
//
//	physx::PxQueryFilterData layersToCollideWith = physx::PxQueryFilterData();
//
//	layersToCollideWith.flags.raise(PxQueryFlag::eDYNAMIC);
//	layersToCollideWith.flags.raise(PxQueryFlag::eSTATIC);
//
//	layersToCollideWith.data.word0 = aLayers;
//
//	PxRaycastHit hitBuffer[256]; // Define a sufficiently large buffer
//	PxRaycastBuffer rayBuffer = PxRaycastBuffer(hitBuffer, 256);
//
//	auto& physScene = MainSingleton::Get()->mySceneManager->GetCurScene()->GetPhysicsScene().myPhysicsScene;
//
//	bool status = physScene->raycast(PXorigin, PXdirection, PXmaxDistance, rayBuffer, PxHitFlag::eMESH_MULTIPLE | PxHitFlag::eDEFAULT, layersToCollideWith);
//
//	outHits.nbHits = rayBuffer.getNbAnyHits();
//
//	for (int i = 0; i < static_cast<int>(rayBuffer.getNbAnyHits()); i++)
//	{
//		auto& pxHit = rayBuffer.getAnyHit(i);
//
//		Hit rayHit;
//
//		rayHit.hitObject = static_cast<GameObject*>(pxHit.actor->userData);
//		rayHit.normal = pxHit.normal;
//		rayHit.position = pxHit.position;
//		rayHit.distanceToHit = pxHit.distance;
//
//		outHits.hits.emplace_back(rayHit);
//	}
//
//	return status;
//}

bool PhysicsEngine::OverlapSphere(physx::PxU32 aLayers, Hits& outHits, float aRadius, CommonUtilities::Vector3<float> aPosition)
{
	PxQueryFilterData filterData(PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC | PxQueryFlag::eNO_BLOCK);

	filterData.data.word0 = aLayers;

	PxOverlapHit hitBuffer[256]; // Define a sufficiently large buffer
	PxOverlapBuffer  hitOverlapBuffer = PxOverlapBuffer(hitBuffer, 256);

	auto& physicsScene = MainSingleton::Get()->activeScene->myPhysicsScene.myPhysicsScene;

	bool overlapDetected = physicsScene->overlap(PxSphereGeometry(aRadius), PxTransform(physx::PxVec3(aPosition)), hitOverlapBuffer, filterData);

	if (overlapDetected)
	{
		for (int i = 0; i < static_cast<int>(hitOverlapBuffer.getNbAnyHits()); i++)
		{
			auto& pxHit = hitOverlapBuffer.getAnyHit(i);

			Hit rayHit;

			rayHit.hitObject = static_cast<GameObject*>(pxHit.actor->userData);
			rayHit.normal = CommonUtilities::Vector3<float>(0, 1, 0);
			rayHit.position = aPosition;
			rayHit.distanceToHit = 0;

			outHits.hits.emplace_back(rayHit);
		}
	}

	return overlapDetected;
}

bool PhysicsEngine::OverlapBox(physx::PxU32 /*aLayers*/, Hits& /*outHits*/, const CommonUtilities::Vector3<float>& /*aSize*/, const CommonUtilities::Vector3<float>& /*aRotation*/, const CommonUtilities::Vector3<float>& /*aPosition*/)
{
	/*PxQueryFilterData filterData(PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC | PxQueryFlag::eNO_BLOCK);

	filterData.data.word0 = aLayers;

	PxOverlapHit hitBuffer[256];
	PxOverlapBuffer hitOverlapBuffer = PxOverlapBuffer(hitBuffer, 256);

	auto& physicsScene = MainSingleton::Get()->activeScene->myPhysicsScene.myPhysicsScene;

	PxQuat quat(aRotation.ToQuarternion());

	bool overlapDetected = physicsScene->overlap(PxBoxGeometry(aSize), PxTransform(PxVec3(aPosition), quat), hitOverlapBuffer, filterData);

	if (overlapDetected)
	{
		for (int i = 0; i < static_cast<int>(hitOverlapBuffer.getNbAnyHits()); i++)
		{
			auto& pxHit = hitOverlapBuffer.getAnyHit(i);

			Hit rayHit;

			rayHit.hitObject = static_cast<GameObject*>(pxHit.actor->userData);
			rayHit.normal = CU::Vector3f(0, 1, 0);
			rayHit.position = aPosition;
			rayHit.distanceToHit = 0;

			outHits.hits.emplace_back(rayHit);
		}
	}

	return overlapDetected;*/

	return false;
}

inline void CollisionCallback::onContact(const PxContactPairHeader& /*pairHeader*/, const PxContactPair* /*pairs*/, PxU32 nbPairs)
{
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		/*const PxContactPair& cp = pairs[i];

		PxActor* actor1 = pairHeader.actors[0];
		PxActor* actor2 = pairHeader.actors[1];

		GameObject* actor1GO = static_cast<GameObject*>(actor1->userData);
		GameObject* actor2GO = static_cast<GameObject*>(actor2->userData);

		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			actor1GO->CallOnCollisionEnter(actor2GO);
			actor2GO->CallOnCollisionEnter(actor1GO);
		}

		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			if (actor1GO)
			{
				actor1GO->CallOnCollisionExit(actor2GO);
			}
			else if (actor2GO)
			{
				actor2GO->CallOnCollisionExit(actor1GO);
			}
		}

		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
		{
			actor1GO->CallOnCollision(actor2GO);
			actor2GO->CallOnCollision(actor1GO);
		}*/
	}
}

inline void CollisionCallback::onTrigger(PxTriggerPair* /*pairs*/, PxU32 count)
{
	for (PxU32 i = 0; i < count; i++)
	{
		/*const PxTriggerPair& cp = pairs[i];

		PxActor* actor1 = pairs->otherActor;
		PxActor* actor2 = pairs->triggerActor;

		GameObject* actor1GO = static_cast<GameObject*>(actor1->userData);
		GameObject* actor2GO = static_cast<GameObject*>(actor2->userData);

		if (cp.status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			if (actor2GO)
			{
				actor2GO->CallOnTriggerEnter(actor1GO);
			}
		}

		if (cp.status & PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			if (actor2GO)
			{
				actor2GO->CallOnTriggerExit(actor1GO);
			}
		}*/
	}
}

inline CustomInputData::CustomInputData(const std::vector<uint8_t>& inputData) : data(inputData) {}

inline uint32_t CustomInputData::getLength() const {
	return static_cast<uint32_t>(data.size());
}

inline void CustomInputData::seek(uint32_t offset) {
	currentPos = std::min(offset, getLength());
}

inline uint32_t CustomInputData::tell() const {
	return currentPos;
}

inline uint32_t CustomInputData::read(void* dest, uint32_t count) {
	uint32_t bytesToRead = std::min(count, getLength() - currentPos);
	memcpy(dest, data.data() + currentPos, bytesToRead);
	currentPos += bytesToRead;
	return bytesToRead;
}
