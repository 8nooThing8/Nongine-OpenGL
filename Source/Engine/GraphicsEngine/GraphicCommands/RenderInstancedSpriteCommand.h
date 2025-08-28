#pragma once

#include <utility>

#include "../Engine/GraphicsEngine/GraphicsCommandList.h"

#include <Vector/Vector3.hpp>
#include <Matrix/Matrix4x4.hpp>

namespace non
{
	class Sprite;
}

class Sprite2DRenderer;
struct SpriteRender;

class Material;

class RenderInstancedSpriteCommand : public GraphicsCommandBase
{
	friend class GraphicsCommandList;

	RenderInstancedSpriteCommand() = default;
	RenderInstancedSpriteCommand(
		const std::pair<Sprite2DRenderer*, std::vector<SpriteRender*>>* aInstanceArray,
		Material* aMaterial,
		bool useTexture);

	void Execute() override;
	void Destroy() override;

private:
	const std::pair<Sprite2DRenderer*, std::vector<SpriteRender*>>* myInstanceArray;
	Material* myMaterial;
	bool myUseTexture;

	const int sizeOfMatVec = sizeof(CU::Matrix4x4<float>) + sizeof(CU::Vector3f);
};