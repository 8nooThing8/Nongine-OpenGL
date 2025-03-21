#include "../../Engine.pch.h"

#include "RenderParticleCommand.h"
#include "../Components/Particles/ParticleRenderer.h"

#include "../Engine/GameEngine/Components/Particles/Particle.h"

RenderParticleCommand::RenderParticleCommand(ParticleRenderer* aParticleRenderer) : myRenderer(aParticleRenderer)
{
	//std::cout 	<< "stuff";
}

void RenderParticleCommand::Execute()
{
	if (myRenderer)
	{
	myRenderer->Draw();

	}
}

void RenderParticleCommand::Destroy()
{

}