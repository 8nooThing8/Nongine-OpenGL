#include "../../Engine.pch.h"

#include "RenderTrailCommand.h"
#include "../Components/Trail/TrailRenderer.h"

#include "../Engine/GameEngine/Components/Particles/Particle.h"

RenderTrailCommand::RenderTrailCommand(TrailRenderer* aParticleRenderer, Trail* aTrail) : myRenderer(aParticleRenderer), myTrail(aTrail)
{
	//std::cout 	<< "stuff";
}

void RenderTrailCommand::Execute()
{
	myRenderer->Draw(myTrail);
}

void RenderTrailCommand::Destroy()
{
	
}