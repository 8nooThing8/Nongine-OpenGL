#include "../../Engine.pch.h"

#include "RenderMeshCommand.h"
#include "../Components/MeshRenderer.h"

RenderMeshCommand::RenderMeshCommand(MeshRenderer* aMeshRenderer) : myRenderer(aMeshRenderer)
{
}

void RenderMeshCommand::Execute()
{
	myRenderer->Draw();
}

void RenderMeshCommand::Destroy()
{
	
}