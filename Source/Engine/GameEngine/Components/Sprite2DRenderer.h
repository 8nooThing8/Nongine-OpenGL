#pragma once

#include "Component.h"
#include "../../GraphicsEngine/Material/Material.h"
#include "../Engine/GraphicsEngine/Sprite.h"
#include "Engine/GraphicsEngine/GraphicsEngine.h"

class Sprite2DRenderer : public Component
{
public:
	~Sprite2DRenderer() override = default;
	Sprite2DRenderer() = default;
	Sprite2DRenderer(const Material& aMaterial, int aLayer);

	void Update(float) override;
	void Start() override;
	void AddToGraphicsEngine();

	Material& GetMaterial();

	int GetLayer() { return myLayer; }

	bool isVisible;
	SpriteRender renderer;

private:
	Material myMaterial;
	int myLayer = 0;
};