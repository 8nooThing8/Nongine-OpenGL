#include "RenderInstancedSpriteCommand.h"


#include "../Engine/GameEngine/GameCamera.h"

#include <../Engine/GameEngine/Components/Sprite2DRenderer.h>
#include <../Engine/GraphicsEngine/Material/Material.h>
#include <../Engine/GraphicsEngine/Sprite.h>
#include <../Engine/GameEngine/Components/GameObject.h>

#include "../Engine/GraphicsEngine/GraphicsEngine.h"
#include <glad/glad.h>

RenderInstancedSpriteCommand::RenderInstancedSpriteCommand(
	const std::pair<Sprite2DRenderer*, std::vector<SpriteRender*>>* aInstanceArray,
	Material* aMaterial,
	bool useTexture)
	:
	myInstanceArray(aInstanceArray),
	myMaterial(aMaterial),
	myUseTexture(useTexture)
{

}

void RenderInstancedSpriteCommand::Execute()
{
	Material* newMat;

	if (!myMaterial)
		newMat = &myInstanceArray->first->GetMaterial();
	else
		newMat = myMaterial;

	Material& material = *newMat;

	material.Use(true, myUseTexture);
	material.SetVector4("material.albedo", material.GetAlbedo());
	material.SetMatrix4x4("invViewMatrix", CU::GameCamera::main->GetInverse());

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const auto& spriteRenders = myInstanceArray->second;

	int totalRenderCount = 0;

	for (int i = 0; i < spriteRenders.size(); i++)
	{
		const auto& spriteRender = spriteRenders[i];

		if (!spriteRender->isVisible)
			continue;

		std::memcpy(non::Sprite::transformMatricies[totalRenderCount].transform.myData, spriteRender->transform.myData, sizeof(CU::Matrix4x4<float>));
		std::memcpy(&non::Sprite::transformMatricies[totalRenderCount].pixelPick.x, &spriteRender->pixelPickID.x, sizeof(CU::Vector3f));

		totalRenderCount++;
	}

	GraphicsEngine::Get().GetSprite().DrawSpriteInstanced(totalRenderCount);
}

void RenderInstancedSpriteCommand::Destroy()
{

}