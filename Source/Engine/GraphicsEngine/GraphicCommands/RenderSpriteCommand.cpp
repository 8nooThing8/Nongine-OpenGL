#include "RenderSpriteCommand.h"


#include "../Engine/GameEngine/GameCamera.h"

#include <../Engine/GraphicsEngine/Material/Material.h>
#include <../Engine/GraphicsEngine/Sprite.h>
#include <../Engine/GameEngine/Components/GameObject.h>

#include "../Engine/GraphicsEngine/GraphicsEngine.h"
#include <glad/glad.h>

RenderSpriteCommand::RenderSpriteCommand(GameObject* aGameObject, Material* aMaterial, bool useTexture) : myGameObject(aGameObject), myMaterial(aMaterial), myUseTexture(useTexture)
{

}

void RenderSpriteCommand::Execute()
{
	myMaterial->Use(true, myUseTexture);

	const CU::Vector3f& pixelpickColor(myGameObject->GetPixelPickRGB());
	myMaterial->SetVector3("pixelPickingID", pixelpickColor);

	myMaterial->SetMatrix4x4("invViewMatrix", CU::GameCamera::main->GetInverse());

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const auto& transform = myGameObject->transform;
	const CU::Matrix4x4<float>& objectMatrix = transform->GetMatrix();

	myMaterial->SetMatrix4x4("objectBuffer.OB_matrix", objectMatrix);

	myMaterial->SetVector4("material.albedo", myMaterial->GetAlbedo());

	GraphicsEngine::Get().GetSprite().DrawSprite();
}

void RenderSpriteCommand::Destroy()
{

}