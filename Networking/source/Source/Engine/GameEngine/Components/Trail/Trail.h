#pragma once

#include <Timer.h>
#include <Vector/Vector.hpp>

#include "TrailSystem.h"

#include <random>

#include "../GameObject.h"

#include <vector>

class Trail
{
public:
	void Reset()
	{
		for (int i = 1; i < myPoints.size() - 1; i++)
		{
			delete myPoints[i];
		}

		myPoints.clear();
	}

	Trail() = default;
	Trail(TrailSystem* aParticleSystem)
	{
		myTrailSystem = aParticleSystem;

		std::vector<CommonUtilities::Vector4<float>> colors;

		colors.push_back(CommonUtilities::Vector4<float>(1, 1, 1, 1));

		myParticleColors.push_back(colors);

		CommonUtilities::Vector4<float> position = myTrailSystem->gameObject->transform->position;

		position.w = 1;

		lastPosition = position;
	}
	~Trail() = default;

	void RenderSphereShapeImGUI()
	{
		myTrailIndecies.reserve(16384);
		myTrailVertecies.reserve(16384);
	}

	void RenderImGUI()
	{
		ImGui::Dummy(ImVec2(10, 0)); ImGui::SameLine();
		ImGui::Text("Trail properties");

		ImGui::Dummy(ImVec2(0, 5));

		ImGui::Dummy(ImVec2(30, 0)); ImGui::SameLine();
		bool thickness = ImGui::CollapsingHeader("Thickness");

		if (thickness)
		{
			ImGui::Dummy(ImVec2(30, 0)); ImGui::SameLine();
			ImGui::DragFloat("##Thickness", &myThickness, 0.01f);
		}

		ImGui::Dummy(ImVec2(30, 0)); ImGui::SameLine();
		bool LifetimeHader = ImGui::CollapsingHeader("Lifetime");

		if (LifetimeHader)
		{
			ImGui::Dummy(ImVec2(30, 0)); ImGui::SameLine();
			ImGui::DragFloat("##Lifetime", &myLifeTime, 0.01f);
		}

		/*ImGui::Dummy(ImVec2(30, 0)); ImGui::SameLine();
		bool colors = ImGui::CollapsingHeader("Colors");

		std::stringstream stream;

		if (colors)
		{
			ImGui::Dummy(ImVec2(30, 0)); ImGui::SameLine();

			if (ImGui::Button("Add"))
			{
				myColorRandomizer = std::uniform_int_distribution<int>(0, static_cast<int>(myParticleColors.size()));

				std::vector<CommonUtilities::Vector4<float>> colorsI;

				colorsI.push_back(CommonUtilities::Vector4<float>(1, 1, 1, 1));

				myParticleColors.push_back(colorsI);
			}

			for (int j = 0; j < myParticleColors.size(); j++)
			{
				ImGui::Dummy(ImVec2(30, 0)); ImGui::SameLine();

				for (int i = 0; i < myParticleColors[j].size(); i++)
				{
					auto& color = myParticleColors[j][i];

					stream << "##ParticleColors" << i;

					ImVec4 vec(color.x, color.y, color.z, color.w);
					bool colorButton = ImGui::ColorButton(stream.str().c_str(), vec, 0, ImVec2(20, 20));
					ImGui::SameLine();

					if (colorButton)
					{
						ImGui::OpenPopup(stream.str().c_str());
					}

					if (ImGui::BeginPopup(stream.str().c_str()))
					{
						ImGui::ColorPicker4(stream.str().c_str(), &color[0]);

						ImGui::EndPopup();
					}
				}

				stream = std::stringstream();
				stream << "+" << "##AddColor" << j;

				if (ImGui::Button(stream.str().c_str(), ImVec2(20, 20)))
				{
					myParticleColors[j].push_back(CommonUtilities::Vector4<float>(1, 1, 1, 1));
				}
			}
		}*/

		ImGui::Dummy(ImVec2(0, 10));
		ImGui::Dummy(ImVec2(10, 0)); ImGui::SameLine();

		ImGui::Text("Emitter properties");

		ImGui::Dummy(ImVec2(0, 5));

		ImGui::Dummy(ImVec2(30, 0)); ImGui::SameLine();
		ImGui::Text("Min move distance to dispense tail");

		ImGui::Dummy(ImVec2(30, 0)); ImGui::SameLine();
		ImGui::DragFloat("##Min move distance to despondence tail", &myMovementThreshold, 0.001f);
	}

	CommonUtilities::Matrix4x4<float> GetFinalTransform(
		const TrailPoint* aParticle,
		CommonUtilities::Vector3<float> size)
	{
		CommonUtilities::Matrix4x4<float> returnMX;

		// Step 1: Scale the matrix
		returnMX(1, 1) = size.x;
		returnMX(2, 2) = size.y;
		returnMX(3, 3) = size.z;

		// Step 2: Compute direction to camera
		//CommonUtilities::Vector3<float> toCamera = VertexToCamera.GetNormalized();

		// Step 3: Align the trail point to face the camera
		// Assuming the "up" direction is (0, 1, 0), create a rotation matrix.
		//CommonUtilities::Vector3<float> up(0, 1, 0);
		CommonUtilities::Matrix4x4<float> faceCameraRotation = CommonUtilities::Matrix4x4<float>::CreateRotation(aParticle->rotation);// = CommonUtilities::Matrix4x4<float>::CreateLookAt(VertexPos, static_cast<InspectorCamera*>(Camera::main)->position, up);

		// Step 4: Set position
		faceCameraRotation.SetPosition(aParticle->position);

		// Step 5: Combine scaling and rotation
		returnMX = returnMX * faceCameraRotation;

		return returnMX;
	}

	void UpdateTrail(bool updateBuffers)
	{
		PIXScopedEvent(PIX_COLOR_INDEX(3), L"Update trail");
		
		{
			PIXScopedEvent(PIX_COLOR_INDEX(6), L"Update trailpoints");

			for (int vertexPointIndex = 1; vertexPointIndex < myPoints.size() - 1; vertexPointIndex++)
			{
				if (myPoints[vertexPointIndex]->Update())
				{
					for (int i = 1; i < myPoints.size() - 1; i++)
					{
						myPoints[i] = myPoints[i + 1];
					}

					//delete myPoints.back();

					myPoints.pop_back();
				}
			}
		}


		std::vector<TrailVertex> trailVertecies;
		std::vector<unsigned> trailIndecies;

		TrailVertex vertex;

		static const std::array trailVertexOffsets
		{
			TrailVertex(CommonUtilities::Vector4<float>(-1, -1, 0, 1), CommonUtilities::Vector2<float>(0, 1)), // Bottom-left
			TrailVertex(CommonUtilities::Vector4<float>(1, -1, 0, 1), CommonUtilities::Vector2<float>(1, 1)),  // Bottom-right
			TrailVertex(CommonUtilities::Vector4<float>(1, 1, 0, 1), CommonUtilities::Vector2<float>(1, 0)),   // Top-right
			TrailVertex(CommonUtilities::Vector4<float>(-1, 1, 0, 1), CommonUtilities::Vector2<float>(0, 0))   // Top-left
		};

		static const std::array indices =
		{
		3, 2, 0,
		2, 1, 0
		};

		static const std::array leftRight =
		{
		-1.f, 1.f
		};

		PIXScopedEvent(PIX_COLOR_INDEX(3), L"Add trail vertecies");

		if (myPoints.size() > 1)
		{
			const TrailPoint* trailPoint2 = myPoints[1];

			for (int i = 0; i < 4; i++)
			{
				vertex.Position = trailPoint2->position;

				//CU::Matrix4x4<float> finalMatrix = GetFinalTransform(trailPoint, CommonUtilities::Vector3<float>(thickness, 1, 1));

				vertex.Position = vertex.Position;// * finalMatrix;

				vertex.UV = trailVertexOffsets[0].UV;
				trailVertecies.push_back(vertex);
			}

			for (const auto& index : indices)
			{
				trailIndecies.push_back(index);
			}

			for (int vertexPointIndex = 1; vertexPointIndex < myPoints.size(); vertexPointIndex++)
			{
				//const TrailPoint* trailPointPrevious = myPoints[vertexPointIndex - 1];
				const TrailPoint* trailPoint = myPoints[vertexPointIndex];

				PIXScopedEvent(PIX_COLOR_INDEX(3), L"Calculate trail thickness");

				float trailThickness = trailPoint->thickness * ((trailPoint->timeAlive - trailPoint->LifeTime) / trailPoint->LifeTime);


				float halfThickness = trailThickness * 0.5f;

				trailVertecies.push_back(trailVertecies[(vertexPointIndex * 4) - 1]);
				trailVertecies.push_back(trailVertecies[(vertexPointIndex * 4) - 2]);
				
				PIXScopedEvent(PIX_COLOR_INDEX(7), L"Connect verts");

				for (int i = 0; i < 2; i++)
				{
					//auto towardsNext = trailPoint->position - trailPointNext->position;

					vertex.Position = trailPoint->position + CommonUtilities::Vector4<float>(halfThickness, 0, 0, 0) * leftRight[i];

					vertex.Position = vertex.Position;// * finalMatrix;

					vertex.UV = trailVertexOffsets[0].UV;
					trailVertecies.push_back(vertex);
				}

				PIXScopedEvent(PIX_COLOR_INDEX(8), L"Adding the indecies to the trail");
				for (const auto& index : indices)
				{
					trailIndecies.push_back(index + (vertexPointIndex * 4));
				}
			}
		}


		PIXScopedEvent(PIX_COLOR_INDEX(3), L"Update vx and idx buffers");

		if (updateBuffers)
		{
			GraphicsEngine::GetRHI()->UpdateIndexBuffer(myTrailIndecies, myTrailSystem->myTrailRenderer->myIndexBuffer);
			GraphicsEngine::GetRHI()->UpdateVertexBuffer<TrailVertex>(myTrailVertecies, myTrailSystem->myTrailRenderer->myVertexBuffer);
		}


		myTrailVertecies = std::move(trailVertecies);
		myTrailIndecies = std::move(trailIndecies);

	}

	void Update()
	{
		CommonUtilities::Vector4<float> position = myTrailSystem->gameObject->transform->position;

		position.w = 1;

		if ((lastPosition - position).Length() >= myMovementThreshold)
		{
			//CommonUtilities::Vector4<float> towardsNew = (lastPosition - position);

			//towardsNew.Normalize();

			//CommonUtilities::Matrix4x4<float> faceCameraRotation = CommonUtilities::Matrix4x4<float>::CreateLookAt(lastPosition, position, CommonUtilities::Vector3<float>(0, 1, 0));

			//CommonUtilities::Vector4<float> rotation = faceCameraRotation.GetRotationAsQuerternion().ToEuler();

			//rotation.x = 0;
			//rotation.y = 0;
			//rotation.z = 0;
			//rotation.w = 0;

			myTrailSystem->SpawnTrailPoint
			(
				position,
				CommonUtilities::Vector4<float>(0 ,0, 0, 0),
				myThickness,
				CommonUtilities::Vector4<float>(0, 1, 0, 1),
				myLifeTime
			);


		}

		UpdateTrail(true);

		lastPosition = position;
	}

private:
	friend class TrailSystem;
	friend class TrailRenderer;

	CommonUtilities::Vector4<float> lastPosition;

	float myThickness = 10;
	float myLifeTime = 0.3f;

	float myMovementThreshold = 0.1f;

	TrailSystem* myTrailSystem;

	std::vector<TrailPoint*> myPoints;

	std::vector<TrailVertex> myTrailVertecies;
	std::vector<unsigned> myTrailIndecies;

	float timer;
	float myEmitterSpeed = 0.f;

	std::vector<std::vector<CommonUtilities::Vector4<float>>> myParticleColors;

	bool myUseGravity = false;
	bool contineus;
};
