#pragma once

#include "../Engine/GraphicsEngine/GraphicsCommandList.h"

class MeshRenderer;

class RenderMeshCommand : public GraphicsCommandBase
{
	friend class GraphicsCommandList;

	RenderMeshCommand() = default;

	RenderMeshCommand(MeshRenderer* aMeshRenderer);

	void Execute() override;
	void Destroy() override;

private:
	MeshRenderer* myRenderer;
};