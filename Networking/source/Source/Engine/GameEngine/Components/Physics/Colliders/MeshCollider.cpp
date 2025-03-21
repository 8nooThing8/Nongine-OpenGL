#include "../Engine/Engine.pch.h"
#include "MeshCollider.h"

#include "TgaFbxStructs.h"

MeshCollider::MeshCollider(TGA::FBX::Mesh& aMesh) : myMesh(&aMesh), Collider(PhysicsEngine::Get().CreateMeshCollider(&aMesh))
{
}

MeshCollider::MeshCollider()
{
}

void MeshCollider::SetMesh(TGA::FBX::Mesh* aMesh)
{
	myMesh = aMesh;
}

void MeshCollider::Start()
{
	SetShape(PhysicsEngine::Get().CreateMeshCollider(myMesh));

	Collider::Start();
}