#pragma once

#include "../Engine/GraphicsEngine/GraphicsCommandList.h"

namespace non
{
	class Mesh;
}

class GameObject;
class Material;

class RenderMeshCommand : public GraphicsCommandBase
{
	friend class GraphicsCommandList;

	RenderMeshCommand() = default;
	RenderMeshCommand(GameObject* aGameObject, non::Mesh** aMesh, Material* aMaterial);

	void Execute() override;
	void Destroy() override;

private:
	GameObject* myGameObject;
	non::Mesh** myMesh;
	Material* myMaterial;
};