#pragma once

#include "../Engine/GraphicsEngine/GraphicsCommandList.h"

namespace non
{
	class Sprite;
}

class GameObject;
class Material;

class RenderSpriteCommand : public GraphicsCommandBase
{
	friend class GraphicsCommandList;

	RenderSpriteCommand() = default;
	RenderSpriteCommand(GameObject* aGameObject, Material* aMaterial, bool useTexture);

	void Execute() override;
	void Destroy() override;

private:
	GameObject* myGameObject;
	Material* myMaterial;
	bool myUseTexture;
};