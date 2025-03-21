#pragma once
#include "../Component.h"

#include <vector>

#include "Particle.h"

#include <memory>

#include "ParticleRenderer.h"

#include "../../Camera.h"
#include "../../InspectorCamera.h"

class Emitter;



class ParticleSystem : public Component
{
public:

	void Start() override;
	void LateUpdateEditor(float) override;

	void RenderImGUI() override;
	std::shared_ptr<ParticleRenderer> myParticleRenderer;

private:
	std::vector<Emitter*> myEmitters;

	std::vector<Particle*> myParticles;


	int myMaxParticles = 5000;

	bool playing = true;
	bool paused = false;
};

