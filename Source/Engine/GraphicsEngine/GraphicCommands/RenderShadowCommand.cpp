#include "RenderShadowCommand.h"


#include "../Engine/GameEngine/GameCamera.h"

#include <../Engine/GraphicsEngine/Material/Material.h>
#include <../Engine/GraphicsEngine/Mesh.h>
#include <../Engine/GameEngine/Components/GameObject.h>

#include <glad/glad.h>

RenderShadowCommand::RenderShadowCommand(GameObject* aGameObject, non::Mesh** aMesh, Material* aMaterial) : myGameObject(aGameObject), myMesh(aMesh), myMaterial(aMaterial)
{

}

void RenderShadowCommand::Execute()
{
	myMaterial->Use();

	//for (unsigned int i = 0; i < 125; i++)
	//{
	//	material.SetMatrix4x4(("offsets[" + std::to_string(i) + "]"), translations[i]);
	//}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const auto& transform = myGameObject->transform;
	const CU::Matrix4x4<float>& objectMatrix = transform->GetMatrix();
	const CU::Matrix4x4<float>& objectMatrixNoScale = transform->GetMatrixNoScale();

	myMaterial->SetMatrix4x4("objectBuffer.OB_matrix", objectMatrix);
	myMaterial->SetMatrix4x4("objectBuffer.OB_matrixNoScale", objectMatrixNoScale);
	//myMaterial->SetVector3("objectBuffer.OB_scale", transform->GetScale().ToVector3());

	(*myMesh)->DrawMesh();
}

void RenderShadowCommand::Destroy()
{

}