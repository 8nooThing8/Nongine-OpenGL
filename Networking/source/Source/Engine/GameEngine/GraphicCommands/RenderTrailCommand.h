#pragma once

#include "../Engine/GraphicsEngine/GraphicsCommandList.h"

class Trail;
class TrailRenderer;

class RenderTrailCommand : public GraphicsCommandBase
{
	friend class GraphicsCommandList;
public:
	RenderTrailCommand() = default;

	RenderTrailCommand(TrailRenderer* aParticleRenderer, Trail* aParticle);

	void Execute() override;
	void Destroy() override;

private:
	TrailRenderer* myRenderer;
	Trail* myTrail;
};