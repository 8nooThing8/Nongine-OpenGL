#include "../../../Engine.pch.h"

#include "ParticleSystem.h"

#include "../../../GraphicsEngine/GraphicsCommandList.h"
#include "../Engine/GameEngine/MainSingleton.h"
#include "../../GraphicCommands/RenderParticleCommand.h"

#include "Emitter.h"
#include "ImGui/imgui_internal.h"



void ParticleSystem::Start()
{
	myParticleRenderer = std::make_shared<ParticleRenderer>();

	//myEmitters.push_back(new Emitter(this));
	//myEmitters.back()->Init
	//(
	//	CommonUtilities::Vector2<float>(100, 300),
	//	CommonUtilities::Vector2<float>(4, 10),
	//	CommonUtilities::Vector2<float>(0, 1),
	//	CommonUtilities::Vector2<float>(1, 1),
	//	CommonUtilities::Vector3<float>(0, 0, 3.14159f * 0.5f),
	//	700.f,
	//	true
	//);

	myEmitters.push_back(new Emitter(this));
	myEmitters.back()->Init
	(
		CommonUtilities::Vector2<float>(500, 500),
		CommonUtilities::Vector2<float>(7, 10),
		CommonUtilities::Vector2<float>(0, 1),
		CommonUtilities::Vector2<float>(1, 2),
		CommonUtilities::Vector3<float>(0, 0, -3.14159f * 0.5f),
		500.f,
		false
	);

	//myEmitters[0]->myParticleColors[0][0] = CommonUtilities::Vector4<float>(0, 1, 0, 1);
	//myEmitters[0]->myParticleColors[0].push_back(CommonUtilities::Vector4<float>(1, 1, 0, 1));
	//myEmitters[0]->myParticleColors[0].push_back(CommonUtilities::Vector4<float>(0, 0, 1, 0));

	myEmitters[0]->myParticleColors[0][0] = CommonUtilities::Vector4<float>(1, 0, 0, 1);
	myEmitters[0]->myParticleColors[0].push_back(CommonUtilities::Vector4<float>(1, 0, 0, 0));
}

void ParticleSystem::LateUpdateEditor(float)
{
	if (playing)
	{
		for (auto& emitter : myEmitters)
		{
			emitter->Update();
		}
	}

	

	/*for (int i = myParticles.size() - 1; i < myParticles.size(); i++)
	{
		auto& particle = myParticles[i];

		if (!paused)
		{
			if (particle->Update())
			{
				delete particle;
				particle = nullptr;
				myParticles[i] = myParticles.back();
				myParticles.pop_back();

				continue;
			}
		}

		
	}*/
	if (myParticleRenderer)
	{
		MainSingleton::Get()->myForwardCommandList->Enqueue([&]() {
			myParticleRenderer->Draw();
			});
	}
	
}

void ParticleSystem::RenderImGUI()
{
	ImGui::Begin("Particle system simulation", (bool*)false, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse);

	if (ImGui::Button("Play"))
	{
		playing = true;
		paused = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("Pause"))
	{
		paused = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop"))
	{
		playing = false;

		for (auto& particle : myParticles)
		{
			delete particle;
		}

		myParticles.clear();
	}

	ImGui::End();

	/*if (ImGui::Button("Add Emitter"))
	{
		myEmitters.push_back(new Emitter(this));
		myEmitters.back()->Init
		(
			CommonUtilities::Vector2<float>(0, 1),
			CommonUtilities::Vector2<float>(0, 1),
			CommonUtilities::Vector2<float>(0, 1),
			CommonUtilities::Vector2<float>(0, 1),
			1.f
		);
	}*/

	for (const auto& emitter : myEmitters)
	{
		emitter->RenderImGUI();
	}
}
