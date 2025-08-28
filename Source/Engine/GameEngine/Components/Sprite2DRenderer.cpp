#include "Sprite2DRenderer.h"

#include "../Engine/GraphicsEngine/GraphicsEngine.h"
#include "../../GraphicsEngine/GraphicCommands/RenderSpriteCommand.h"
#include <InputHandler.h>

Sprite2DRenderer::Sprite2DRenderer(const Material& aMaterial, int aLayer) : myMaterial(aMaterial), myLayer(aLayer)
{

}

void Sprite2DRenderer::Update(float)
{
	renderer.transform = gameObject->transform->GetMatrix();
	renderer.isVisible = isVisible;
}

void Sprite2DRenderer::Start()
{	
	myMaterial.TestAndAssignDefaultMaterials();
	
	renderer.pixelPickID = gameObject->GetPixelPickRGB();
	
	AddToGraphicsEngine();
}

void Sprite2DRenderer::AddToGraphicsEngine()
{
	GraphicsEngine::Get().PushSpriteRender(this);
}

Material& Sprite2DRenderer::GetMaterial()
{
	return myMaterial;
}
