#pragma once

#include "../Engine/GraphicsEngine/GraphicsCommandList.h"

struct Particle;
class ParticleRenderer;

class RenderParticleCommand : public GraphicsCommandBase
{
	friend class GraphicsCommandList;
public:
	RenderParticleCommand() = default;

	RenderParticleCommand(ParticleRenderer* aParticleRenderer);

	void Execute() override;
	void Destroy() override;

private:
	ParticleRenderer* myRenderer;
};