#include "../Engine/Engine.pch.h"
#include "MeshCollider.h"

#include "../Engine/GameEngine/Components/GameObject.h"

#include "TgaFbxStructs.h"

MeshCollider::MeshCollider(TGA::FBX::Mesh& aMesh) : myMesh(&aMesh), Collider(PhysicsEngine::Get().CreateMeshCollider(&aMesh, 1))
{
}

MeshCollider::MeshCollider() : myMesh(nullptr)
{
}

void MeshCollider::SetMesh(TGA::FBX::Mesh* aMesh)
{
	myMesh = aMesh;

	SetShape(PhysicsEngine::Get().CreateMeshCollider(myMesh, gameObject->transform->scale.x));
}

void MeshCollider::SetMesh(const std::string& aMeshPath)
{
	myMesh = CreateModel(aMeshPath);

	SetShape(PhysicsEngine::Get().CreateMeshCollider(myMesh, gameObject->transform->scale.x));
}

void MeshCollider::Start()
{
	SetShape(PhysicsEngine::Get().CreateMeshCollider(myMesh, gameObject->transform->scale.x));

	Collider::Start();
}

void MeshCollider::HandleSave(rapidjson::Value& aOutJson, rapidjson::Document& aDocument)
{
	Collider::HandleSave(aOutJson, aDocument);

	auto meshVal = rapidjson::Value(myMesh->Name.c_str(), aDocument.GetAllocator());
	aOutJson.AddMember("Mesh", meshVal, aDocument.GetAllocator());
}

void MeshCollider::HandleLoad(const rapidjson::Value& aObj)
{
	Collider::HandleLoad(aObj);

	SetMesh(aObj["Mesh"].GetString());
}