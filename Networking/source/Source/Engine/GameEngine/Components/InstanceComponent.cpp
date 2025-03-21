#pragma once

#include "../Engine/Engine.pch.h"

#include "InstanceComponent.h"

#include "../MainSingleton.h"

#include "../Engine/GameEngine/Components/GameObject.h"

void InstanceComponent::EarlyUpdateEditor(float /*aFloat*/)
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), L"Instanced render update");

	for (int i = 0; i < myInstancedModels.size(); i++)
	{
		CommonUtilities::Matrix4x4<float> mat4;

		mat4.SetScale(myInstancedModelsTransforms[i].scale);
		mat4 *= CommonUtilities::Matrix4x4<float>::CreateRotation(myInstancedModelsTransforms[i].rotation);
		mat4.SetPosition(myInstancedModelsTransforms[i].position);

		myInstancedModels[i] = mat4;
	}

	GraphicsEngine::GetRHI()->UpdateVertexBuffer<CommonUtilities::Matrix4x4<float>>(myInstancedModels, GraphicsEngine::Get().myInstanceBuffer);
}

void InstanceComponent::Start()
{
	if (instancedMesh)
		return;

	instancedMesh = CreateModel("Assets/Models/SM_Chest.fbx");

	myInstanceMaterial.SetShader(L"Assets/Shaders/Deffered_PS.hlsl", L"Assets/Shaders/Default_VS.hlsl", L"Assets/Textures/Chest_C.dds", L"Assets/Textures/Chest_N.dds");

	myInstanceMaterial.SetMetallicTexture(L"Assets/Textures/Chest_M.dds");
	myInstanceMaterial.SetRoughnessTexture(L"Assets/Textures/Chest_M.dds");
	myInstanceMaterial.SetAOTexture(L"Assets/Textures/Chest_M.dds");

	myInstanceMaterialForward.SetShader(L"Assets/Shaders/Red_PS.hlsl", L"Assets/Shaders/Default_VS.hlsl");

	GraphicsEngine::GetRHI()->UpdateVertexBuffer<CommonUtilities::Matrix4x4<float>>(myInstancedModels, GraphicsEngine::Get().myInstanceBuffer);

	MainSingleton::Get()->instanceComponent = this;
}

void InstanceComponent::RenderImGUI()
{
	for (int i = 0; i < myInstancedModels.size(); i++)
	{
		std::stringstream stream;

		stream << "nr: " << i + 1;
		
		if (myInstancedModelsTransforms[i].arrowOpen)
		{
			if (ImGui::ArrowButton(" ", ImGuiDir_Up))
			{
				myInstancedModelsTransforms[i].arrowOpen = false;
			}
		}
		else
		{
			if (ImGui::ArrowButton(" ", ImGuiDir_Down))
			{
				myInstancedModelsTransforms[i].arrowOpen = true;
			}
		}

		ImGui::SameLine();

		ImGui::Text(stream.str().c_str());

		if (myInstancedModelsTransforms[i].arrowOpen)
		{
			std::stringstream positionStream;
			positionStream << "##" << i << "positionInstancemodelTransformID";

			ImGui::Text("Position\t");
			ImGui::SameLine();
			ImGui::DragFloat3(positionStream.str().c_str(), &myInstancedModelsTransforms[i].position.x, 0.1f);

			std::stringstream rotationStream;
			rotationStream << "##" << i << "rotationInstancemodelTransformID";

			ImGui::Text("Rotation\t");
			ImGui::SameLine();
			ImGui::DragFloat3(rotationStream.str().c_str(), &myInstancedModelsTransforms[i].rotation.x, 0.01f);

			std::stringstream scaleStream;
			scaleStream << "##" << i << "scaleInstancemodelTransformID";

			ImGui::Text("Scale  \t");
			ImGui::SameLine();
			ImGui::DragFloat3(scaleStream.str().c_str(), &myInstancedModelsTransforms[i].scale.x, 0.01f);
		}
	}

	if (ImGui::Button("Add##AddNewInstanceModelTransform"))
	{
		myInstancedModelsTransforms.emplace_back();
		myInstancedModels.emplace_back();
	}
}
