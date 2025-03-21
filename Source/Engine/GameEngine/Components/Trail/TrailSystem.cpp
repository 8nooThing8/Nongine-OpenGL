#include "../../../Engine.pch.h"

#include "TrailSystem.h"

#include "../../../GraphicsEngine/GraphicsCommandList.h"
#include "../Engine/GameEngine/MainSingleton.h"

#include "../../GraphicCommands/RenderTrailCommand.h"

#include "TrailPoint.h"
#include "ImGui/imgui_internal.h"

#include "Trail.h"

void TrailSystem::SpawnTrailPoint
(
	const CommonUtilities::Vector4<float>& aPosition,
	const CommonUtilities::Vector4<float>& aRotation,
	float aThickness,
	const CommonUtilities::Vector4<float>& aColor,
	float aLifeTime
)
{
	//if (static_cast<int>(myTrailPoints.size()) >= myMaxParticles || paused || !playing)
	//	return;

	TrailPoint* trailPoint = new TrailPoint();

	trailPoint->position = aPosition;
	trailPoint->rotation = aRotation;
	trailPoint->thickness = aThickness;

	trailPoint->myColor = aColor;

	trailPoint->LifeTime = aLifeTime;

	myTrails[0]->myPoints.emplace_back(std::move(trailPoint));
}

void TrailSystem::Start()
{
	centerZ = gameObject->transform->position.z;
	centerY = gameObject->transform->position.y;

	myTrailRenderer = std::make_shared<TrailRenderer>();

	myTrails.push_back(new Trail(this));

	CommonUtilities::Vector4<float> position = gameObject->transform->position;

	position.w = 1;

	SpawnTrailPoint
	(
		position,
		CommonUtilities::Vector4<float>(0, 0, 0, 0),
		10,
		CommonUtilities::Vector4<float>(0, 1, 0, 1),
		10
	);

	//SpawnTrailPoint
	//(
	//	CommonUtilities::Vector4<float>(0, 10, 10, 1),
	//	CommonUtilities::Vector4<float>(0, 0, 0, 0),
	//	CommonUtilities::Vector4<float>(1, 1, 1, 1),
	//	CommonUtilities::Vector4<float>(0, 1, 0, 1),
	//	10
	//);

	//SpawnTrailPoint
	//(
	//	CommonUtilities::Vector4<float>(20, 10, 10, 1),
	//	CommonUtilities::Vector4<float>(0, 0, 0, 0),
	//	CommonUtilities::Vector4<float>(1, 1, 1, 1),
	//	CommonUtilities::Vector4<float>(0, 1, 0, 1),
	//	10
	//);
	//SpawnTrailPoint
	//(
	//	CommonUtilities::Vector4<float>(40, 30, 15, 1),
	//	CommonUtilities::Vector4<float>(0, 0, 0, 0),
	//	CommonUtilities::Vector4<float>(1, 1, 1, 1),
	//	CommonUtilities::Vector4<float>(0, 1, 0, 1),
	//	10
	//);
	//SpawnTrailPoint
	//(
	//	CommonUtilities::Vector4<float>(0, -40, 10, 1),
	//	CommonUtilities::Vector4<float>(0, 0, 0, 0),
	//	CommonUtilities::Vector4<float>(1, 1, 1, 1),
	//	CommonUtilities::Vector4<float>(0, 1, 0, 1),
	//	10
	//);
	
}

void TrailSystem::LateUpdateEditor(float)
{
	if (!paused)
	{
		for (auto& emitter : myTrails)
		{
			emitter->Update();
		}
	}
	if (playing && !paused)
	{
		float z;
		float y;

		float waveFrequency;

		switch (shape)
		{
		case Circle:
			angle += speed * Time::GetDeltaTime();

			z = centerZ + radius * cos(angle);
			y = centerY + radius * sin(angle);

			gameObject->transform->position.z = z;
			gameObject->transform->position.y = y;
			break;

		case Wave:
			waveFrequency = 4.0f;
			angle += (speed / waveFrequency) * Time::GetDeltaTime();

			z = centerZ + radius * cos(angle);
			y = centerY + radius * sin(waveFrequency * angle);

			gameObject->transform->position.z = z;
			gameObject->transform->position.y = y;
			break;
		case Line:
			angle += speed * Time::GetDeltaTime();

			z = centerZ + radius * cos(angle);

			gameObject->transform->position.z = z;
			break;
		}
	}

	for (int i = 0; i < myTrails.size(); i++)
	{
		auto& trail = myTrails[i];
		
		RenderTrailCommand command = RenderTrailCommand(myTrailRenderer.get(), trail);

		MainSingleton::Get()->myFXCommandList->Enqueue<RenderTrailCommand>(command);
	}
}

std::string TrailSystem::ShapeToString(TrailVisulizerShape aShape)
{
	switch(aShape)
	{
	case Circle:
		return "Circle";
	case Wave:
		return "Wave";
	case Line:
		return "Line";
	}

	return "";
}

void TrailSystem::RenderImGUI()
{
	ImGui::Begin("Trail simulation", (bool*)false, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse);

	if (ImGui::Button("Play"))
	{
		if (!playing)
		{
			centerZ = gameObject->transform->position.z;
			centerY = gameObject->transform->position.y;
		}
		
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
		paused = false;

		gameObject->transform->position.z = centerZ;
		gameObject->transform->position.y = centerY;

		for (const auto& emitter : myTrails)
		{
			emitter->Reset();
		}

		//for (auto& particle : myTrailPoints)
		//{
		//	delete particle;
		//}

		//myTrailPoints.clear();
	}

	ImGui::Text("Spin speed");
	ImGui::DragFloat("##Spin speed", &speed, 0.01f);

	ImGui::Text("Radius");
	ImGui::DragFloat("##Radius", &radius, 0.01f);

	ImGui::Text("Trail movement shape");

	if (ImGui::Button(ShapeToString(shape).c_str()))
		ImGui::OpenPopup("Trail movement shape");

	if (ImGui::BeginPopup("Trail movement shape"))
	{
		if (ImGui::Selectable("Circle"))
		{
			shape = Circle;
		}
		else if (ImGui::Selectable("Wave"))
		{
			shape = Wave;
		}
		else if (ImGui::Selectable("Line"))
		{
			shape = Line;
		}

		ImGui::EndPopup();
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

	for (const auto& emitter : myTrails)
	{
		emitter->RenderImGUI();
	}

	//ImGui::Dummy(ImVec2(0, 0));

	//ImGui::Dummy(ImVec2(30, 0));
	//ImGui::SameLine();
	//ImGui::DragInt("Max particles", &myMaxParticles, 1, 0, 9999999);
}
