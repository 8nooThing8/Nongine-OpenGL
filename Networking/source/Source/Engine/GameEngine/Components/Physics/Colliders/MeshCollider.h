#pragma once
#include "../../Component.h"
#include "../../Physics/Colliders/Collider.h"

namespace TGA
{
	namespace FBX
	{
		struct Mesh;
	}
}

class MeshCollider : public Collider
{
public:
	MeshCollider();
	MeshCollider(MeshCollider&) = default;
	~MeshCollider() override = default;

	MeshCollider(TGA::FBX::Mesh& aMesh);

	void SetMesh(TGA::FBX::Mesh* aMesh);

	void Start() override;

private:
	TGA::FBX::Mesh* myMesh;
};
