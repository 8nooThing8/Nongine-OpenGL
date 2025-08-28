#pragma once

#include "../Engine/GraphicsEngine/GraphicsCommandList.h"

namespace non
{
	class Mesh;
}

class GameObject;
class Material;

class RenderShadowCommand : public GraphicsCommandBase
{
	friend class GraphicsCommandList;

	RenderShadowCommand() = default;
	RenderShadowCommand(GameObject* aGameObject, non::Mesh** aMesh, Material* aMaterial);

	void Execute() override;
	void Destroy() override;

private:
	GameObject* myGameObject;
	non::Mesh** myMesh;
	Material* myMaterial;
};