#include "MeshRenderer.h"

#include "../Engine/GameEngine/MainSingleton.h"
#include "../Engine/GraphicsEngine/GraphicCommands/RenderMeshCommand.h"
#include "../Engine/GraphicsEngine/GraphicsCommandList.h"

#include "../Engine/GraphicsEngine/GraphicsEngine.h"

#include "../Engine/GameEngine/GameCamera.h"

MeshRenderer::MeshRenderer(non::Mesh* aMesh, const Material& aMaterial)
{
	myMesh = aMesh;
	myMaterial = aMaterial;
}

void MeshRenderer::Update(float /*aDeltaTime*/)
{
	if (addMeshes)
		DrawMesh();
}

void MeshRenderer::Start()
{
	myMaterial.TestAndAssignDefaultMaterials();
}

void MeshRenderer::SetMesh(non::Mesh* aMesh)
{
	myMesh = aMesh;
}

void MeshRenderer::SetMaterial(const Material& aMaterial)
{
	myMaterial = aMaterial;
}

Material& MeshRenderer::GetMaterial()
{
	return myMaterial;
}

non::Mesh* MeshRenderer::GetMesh()
{
	return myMesh;
}

non::Mesh** MeshRenderer::GetMeshAdress()
{
	return &myMesh;
}

void MeshRenderer::DrawMesh()
{
	GraphicsEngine::Get().PushForwardRender(this);
}
