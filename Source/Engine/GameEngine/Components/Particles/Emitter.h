#pragma once

#include <Timer.h>
#include <Vector/Vector.hpp>

#include "ParticleSystem.h"

#include <random>

#include "../GameObject.h"

#include <queue>

class Emitter
{
public:
	Emitter() = default;
	Emitter(ParticleSystem* aParticleSystem)
	{
		myParticleSystem = aParticleSystem;

		myDirectionRandomizer = std::uniform_int_distribution<int>(0, 1);
		myColorRandomizer = std::uniform_int_distribution<int>(0, 0);

		myForceDirectionRandomizer = std::uniform_real_distribution<float>(0, 1);
		myAzimuthRandom = std::uniform_real_distribution<float>(0, 6.28318f);

		std::vector<CommonUtilities::Vector4<float>> colors;

		colors.push_back(CommonUtilities::Vector4<float>(1, 1, 1, 1));

		myParticleColors.push_back(colors);
	}
	~Emitter() = default;

	void RenderSphereShapeImGUI()
	{

	}

	void RenderConeShapeImGUI()
	{
		if (ImGui::DragFloat("Cone outer width", &coneOuterWidth, 0.001f, 0))
		{
			if (coneOuterWidth >= 0)
				myForceDirectionRandomizer = std::uniform_real_distribution<float>(0, coneOuterWidth);

		}

		if (ImGui::DragFloat("Cone inner width", &coneInnerWidth, 0.001f, 0))
		{
			if (coneInnerWidth >= 0)
				myForceDirectionRandomizer = std::uniform_real_distribution<float>(0, coneInnerWidth);
		}
	}

	void RenderEmmisionShapeImGUI()
	{
		switch (myEmmisionShape)
		{
		case Sphere:
			RenderSphereShapeImGUI();
			break;
		case Cone:
			RenderConeShapeImGUI();
			break;
		}
	}

	void RenderImGUI()
	{
		std::stringstream stringstream;

		stringstream << "Emitter" << "##" << id;

		if (ImGui::CollapsingHeader(stringstream.str().c_str()))
		{
			ImGui::Dummy(ImVec2(10, 0)); ImGui::SameLine();
			ImGui::Text("Particle properties");

			ImGui::Dummy(ImVec2(0, 5));

			ImGui::Dummy(ImVec2(30, 0)); ImGui::SameLine();
			bool SpeedHeader = ImGui::CollapsingHeader("Speed");

			if (SpeedHeader)
			{
				ImGui::Dummy(ImVec2(30, 0)); ImGui::SameLine();
				bool changedSpeedX = ImGui::DragFloat("Min speed", &mySpeedRange.x, 0.01f, 0, mySpeedRange.y);
				ImGui::Dummy(ImVec2(30, 0)); ImGui::SameLine();
				bool changedSpeedY = ImGui::DragFloat("Max speed", &mySpeedRange.y, 0.01f, mySpeedRange.x, 999999);

				if ((changedSpeedX || changedSpeedY) && mySpeedRange.y >= mySpeedRange.x)
					mySpeedRangeRandom = std::uniform_real_distribution<float>(mySpeedRange.x, mySpeedRange.y);
			}


			ImGui::Dummy(ImVec2(30, 0)); ImGui::SameLine();
			bool SizeHeader = ImGui::CollapsingHeader("Size");

			if (SizeHeader)
			{
				ImGui::Dummy(ImVec2(30, 0)); ImGui::SameLine();
				bool changedSizeX = ImGui::DragFloat("Min size", &mySizeRange.x, 0.01f, -999999, mySizeRange.y);
				ImGui::Dummy(ImVec2(30, 0)); ImGui::SameLine();
				bool changedSizeY = ImGui::DragFloat("Max size", &mySizeRange.y, 0.01f, mySizeRange.x, 999999);

				if ((changedSizeX || changedSizeY) && mySizeRange.y >= mySizeRange.x)
					mySizeRangeRandom = std::uniform_real_distribution<float>(mySizeRange.x, mySizeRange.y);
			}

			ImGui::Dummy(ImVec2(30, 0)); ImGui::SameLine();
			bool RotationHeader = ImGui::CollapsingHeader("Rotation");

			if (RotationHeader)
			{
				ImGui::Dummy(ImVec2(30, 0)); ImGui::SameLine();
				bool changedRotationX = ImGui::DragFloat("Min rotation", &myRotationRange.x, 0.01f, -999999, myRotationRange.y);
				ImGui::Dummy(ImVec2(30, 0)); ImGui::SameLine();
				bool changedRotationY = ImGui::DragFloat("Max rotation", &myRotationRange.y, 0.01f, myRotationRange.x, 999999);

				if ((changedRotationX || changedRotationY) && myRotationRange.y >= myRotationRange.x)
					myRotationRangeRandom = std::uniform_real_distribution<float>(myRotationRange.x, myRotationRange.y);
			}

			ImGui::Dummy(ImVec2(30, 0)); ImGui::SameLine();
			bool LifetimeHader = ImGui::CollapsingHeader("Lifetime");

			if (LifetimeHader)
			{
				ImGui::Dummy(ImVec2(30, 0)); ImGui::SameLine();
				bool changedLifeTimeX = ImGui::DragFloat("Min lifetime", &myLifeTimeRange.x, 0.01f, -999999, myLifeTimeRange.y);
				ImGui::Dummy(ImVec2(30, 0)); ImGui::SameLine();
				bool changedLifeTimeY = ImGui::DragFloat("Max lifetime", &myLifeTimeRange.y, 0.01f, myLifeTimeRange.x, 999999);

				if ((changedLifeTimeX || changedLifeTimeY) && myLifeTimeRange.y >= myLifeTimeRange.x)
					myLifeTimeRangeRandom = std::uniform_real_distribution<float>(myLifeTimeRange.x, myLifeTimeRange.y);
			}

			ImGui::Dummy(ImVec2(30, 0)); ImGui::SameLine();
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
			}

			ImGui::Dummy(ImVec2(30, 0)); ImGui::SameLine();

			if (ImGui::Button("Emission shape"))
				ImGui::OpenPopup("Emission shape");

			ImGui::Dummy(ImVec2(30, 0)); ImGui::SameLine();

			if (ImGui::BeginPopup("Emission shape"))
			{
				if (ImGui::Selectable("Sphere"))
				{
					myEmmisionShape = Sphere;
				}
				else if (ImGui::Selectable("Cone"))
				{
					myEmmisionShape = Cone;
				}

				ImGui::EndPopup();
			}

			RenderEmmisionShapeImGUI();

			ImGui::Dummy(ImVec2(0, 10));
			ImGui::Dummy(ImVec2(10, 0)); ImGui::SameLine();

			ImGui::Text("Emitter properties");

			ImGui::Dummy(ImVec2(0, 5));

			ImGui::Dummy(ImVec2(30, 0)); ImGui::SameLine();
			ImGui::DragFloat("Emitter speed", &myEmitterSpeed, 1.f);

			ImGui::Dummy(ImVec2(0, 0));

			ImGui::Dummy(ImVec2(30, 0));
			ImGui::SameLine();
			ImGui::DragInt("Max particles", &myMaxParticles, 1, 0, 9999999);
		}
	}

	void Init
	(
		CommonUtilities::Vector2<float> aSpeedRange,
		CommonUtilities::Vector2<float> aSizeRange,
		CommonUtilities::Vector2<float> aRotationRange,
		CommonUtilities::Vector2<float> aLifeTimeRange,
		CommonUtilities::Vector3<float> aRotation,
		float aEmitterSpeed,
		bool aUseGravity
	)
	{
		static int staticID = 0;

		id = staticID++;

		mySpeedRange = aSpeedRange;
		mySizeRange = aSizeRange;
		myRotationRange = aRotationRange;
		myLifeTimeRange = aLifeTimeRange;
		myRotation = aRotation;

		myUseGravity = aUseGravity;

		myMt = std::mt19937(static_cast<int>(time(0)));

		mySpeedRangeRandom = std::uniform_real_distribution<float>(mySpeedRange.x, mySpeedRange.y);
		mySizeRangeRandom = std::uniform_real_distribution<float>(mySizeRange.x, mySizeRange.y);
		myRotationRangeRandom = std::uniform_real_distribution<float>(myRotationRange.x, myRotationRange.y);
		myLifeTimeRangeRandom = std::uniform_real_distribution<float>(myLifeTimeRange.x, myLifeTimeRange.y);

		myEmitterSpeed = aEmitterSpeed;
	}

	void SpawnParticle
	(
		const CommonUtilities::Vector4<float>& aPosition,
		const CommonUtilities::Vector4<float>& aColor,
		const CommonUtilities::Vector3<float>& aVelocity,
		float aLifetime,
		float aAngle,
		const CommonUtilities::Vector2<float>& aSize,
		const CommonUtilities::Vector4<float>& aChannelMask,
		bool newParticle
	)
	{
		newParticle;
		/*if (static_cast<int>(myParticles.size()) >= myMaxParticles || paused || !playing)
			return;*/

		ParticleVertex particleVertex;

		particleVertex.Position = aPosition;
		particleVertex.Color = aColor;
		particleVertex.Velocity = aVelocity;
		particleVertex.Lifetime = aLifetime;
		particleVertex.Angle = aAngle;
		particleVertex.Size = aSize;
		particleVertex.ChannelMask = aChannelMask;

		maxLifeTime = particleVertex.Lifetime;

		myParticles.emplace_back(std::move(particleVertex));
	}

	void Update()
	{
		PIXScopedEvent(PIX_COLOR_INDEX(1), L"Particle spawner");

		if (myParticles.size() < static_cast<int>(myMaxParticles))
		{
			timer += CommonUtilities::Time::GetDeltaTime();
			float deltaTime = CommonUtilities::Time::GetDeltaTime();

			// myEmitterSpeed now represents particles per second (PPS)
			float particlesPerSecond = myEmitterSpeed;

			// Calculate how many particles should be emitted this frame
			float particlesToEmit = particlesPerSecond * deltaTime;
			int totalParticlesToEmit = static_cast<int>(particlesToEmit);

			// Keep track of fractional particles for accurate emission
			static float particleEmissionRemainder = 0.0f;
			particleEmissionRemainder += particlesToEmit - totalParticlesToEmit;

			// Emit an additional particle if the remainder exceeds 1
			if (particleEmissionRemainder >= 1.0f)
			{
				totalParticlesToEmit++;
				particleEmissionRemainder -= 1.0f;
			}

			// Emit particles
			for (int i = 0; i < totalParticlesToEmit; ++i)
			{
				float randomSpeedX = mySpeedRangeRandom(myMt);
				float randomSpeedY = mySpeedRangeRandom(myMt);
				float randomSpeedZ = mySpeedRangeRandom(myMt);

				int randomDirectionX = myDirectionRandomizer(myMt);
				int randomDirectionY = myDirectionRandomizer(myMt);
				int randomDirectionZ = myDirectionRandomizer(myMt);

				randomDirectionX = randomDirectionX ? 1 : -1;
				randomDirectionY = randomDirectionY ? 1 : -1;
				randomDirectionZ = randomDirectionZ ? 1 : -1;

				CommonUtilities::Vector3<int> randomDirection(randomDirectionX, myEmmisionShape == Sphere ? randomDirectionY : 1, randomDirectionZ);

				randomDirection.Normalize();

				float randomDirectionSpeed = myForceDirectionRandomizer(myMt);

				float outerConeAngleRadians = coneOuterWidth * (3.14159265359f / 180.0f);
				float innerConeAngleRadians = coneInnerWidth * (3.14159265359f / 180.0f);

				float randomAngle = innerConeAngleRadians + randomDirectionSpeed * (outerConeAngleRadians - innerConeAngleRadians);
				float randomAzimuth = myAzimuthRandom(myMt) * 3.14159265359f * 2.0f;

				float sinTheta = sin(randomAngle);
				float cosTheta = cos(randomAngle);

				CommonUtilities::Vector3<float> directionToEmit(1.f, 1.f, 1.f);

				if (myEmmisionShape == Cone)
				{
					directionToEmit.x = sinTheta * cos(randomAzimuth);
					directionToEmit.y = cosTheta;
					directionToEmit.z = sinTheta * sin(randomAzimuth);
				}
				else if (myEmmisionShape == Sphere)
				{
					directionToEmit.x = myForceDirectionRandomizer(myMt);
					directionToEmit.y = myForceDirectionRandomizer(myMt);
					directionToEmit.z = myForceDirectionRandomizer(myMt);
				}

				directionToEmit.Normalize();

				CommonUtilities::Vector4<float> randomForce
				(
					randomSpeedX * directionToEmit.x,
					randomSpeedY * directionToEmit.y,
					randomSpeedZ * directionToEmit.z,
					0.f
				);

				randomForce = randomForce * CommonUtilities::Vector4<float>(static_cast<float>(randomDirection.x), static_cast<float>(randomDirection.y), static_cast<float>(randomDirection.z), 1);

				CommonUtilities::Matrix4x4<float> rotationMatrix = CommonUtilities::Matrix4x4<float>::CreateRotation(myParticleSystem->gameObject->transform->rotation + CommonUtilities::Vector4<float>(myRotation, 0));
				randomForce = randomForce * rotationMatrix;

				float randomSize = mySizeRangeRandom(myMt);
				float randomRotation = myRotationRangeRandom(myMt);
				float randomLifeTime = myLifeTimeRangeRandom(myMt);
				//int particleColorIndex = myColorRandomizer(myMt);

				//if (myParticleSystem->gameObject->GetParent())
				//	myParticleSystem->gameObject->transform->GetObjBuffer().world *= myParticleSystem->gameObject->GetParent()->transform->GetObjBuffer().world;


				/*bool newParticle = true;

				if (myMaxParticles >= myParticles.size() + myDeadParticles.size())
				{
					newParticle = false;
				}*/


				SpawnParticle
				(
					CommonUtilities::Vector4<float>(myParticleSystem->gameObject->transform->GetObjBuffer().world.GetPosition()),
					CommonUtilities::Vector4<float>(0, 0, 0, 1),
					CommonUtilities::Vector3<float>(randomForce),
					randomLifeTime,
					randomRotation,
					CommonUtilities::Vector2<float>(randomSize, randomSize),
					CommonUtilities::Vector4<float>(0, 0, 0, 0),
					false
				);

			}
		}

		for (size_t i = 0; i < myParticles.size(); i++)
		{
			if (myParticles[i].Lifetime <= 0)
			{
				//myDeadParticles.push(std::move(myParticles[i]));

				myParticles[i] = myParticles.back();
				myParticles.pop_back();

				continue;
			}

			float deltaTime = CommonUtilities::Time::GetDeltaTime();

			myParticles[i].Lifetime -= Time::GetDeltaTime();

			//rotation += angularVelocity * deltaTime;

			myParticles[i].Velocity -= CommonUtilities::Vector3<float>(10, 10 + (static_cast<int>(myUseGravity) * 9.81f) * 25, 10) * deltaTime;

			//myParticles[i].Velocity.x = CommonUtilities::Max(myParticles[i].Velocity.x, 0.f);
			//myParticles[i].Velocity.z = CommonUtilities::Max(myParticles[i].Velocity.z, 0.f);

			//position.y += (static_cast<int>(useGravity) * 9.81f) * deltaTime;

			myParticles[i].Position += myParticles[i].Velocity * deltaTime;

			//myParticles[i].Size *= myParticles[i].Lifetime / maxLifeTime;



			float t = (maxLifeTime - myParticles[i].Lifetime) / maxLifeTime;  // Normalized lifetime from 0 to 1
			size_t colorCount = myParticleColors[0].size();

			// Scale `t` to span across the color array
			float colorPick = t * (colorCount - 1);  // Range [0, colorCount-1]

			// Get indices for color interpolation
			int lerpFromColorIndex = static_cast<int>(std::floor(colorPick));
			int lerpToColorIndex = static_cast<int>(std::ceil(colorPick));

			// Clamp indices to stay within array bounds
			lerpFromColorIndex = std::clamp(lerpFromColorIndex, 0, static_cast<int>(colorCount) - 1);
			lerpToColorIndex = std::clamp(lerpToColorIndex, 0, static_cast<int>(colorCount) - 1);

			// Colors at the two indices
			CommonUtilities::Vector4<float> colorFrom = myParticleColors[0][lerpFromColorIndex];
			CommonUtilities::Vector4<float> colorTo = myParticleColors[0][lerpToColorIndex];

			// Interpolation factor for smooth color transition
			float lerpFactor = colorPick - lerpFromColorIndex;
			CommonUtilities::Vector4<float> color = colorFrom * (1.0f - lerpFactor) + colorTo * lerpFactor;

			myParticles[i].Color = color;

		}

		myParticleSystem->myParticleRenderer->myVerticies.insert(myParticleSystem->myParticleRenderer->myVerticies.end(), myParticles.begin(), myParticles.end());
	}

private:
	friend class ParticleSystem;

	enum EmisisonShape
	{
		Sphere = 1 << 0,
		Cone = 1 << 1
	};

	EmisisonShape myEmmisionShape = Cone;

	float maxLifeTime;

	// Shape Emission
		// Cone
	float coneInnerWidth = 20;
	float coneOuterWidth = 45;

	std::vector<ParticleVertex> myParticles;
	std::queue<ParticleVertex> myDeadParticles;

	ParticleSystem* myParticleSystem;

	float timer;

	float myEmitterSpeed = 0.f;
	int myMaxParticles = 3000;

	CommonUtilities::Vector2<float> mySpeedRange;
	CommonUtilities::Vector2<float> mySizeRange;
	CommonUtilities::Vector2<float> myRotationRange;
	CommonUtilities::Vector2<float> myLifeTimeRange;

	CommonUtilities::Vector3<float> myRotation;

	std::vector<std::vector<CommonUtilities::Vector4<float>>> myParticleColors;

	std::mt19937 myMt;

	std::uniform_int_distribution<int> myDirectionRandomizer;
	std::uniform_int_distribution<int> myColorRandomizer;

	std::uniform_real_distribution<float> mySpeedRangeRandom;
	std::uniform_real_distribution<float> mySizeRangeRandom;
	std::uniform_real_distribution<float> myRotationRangeRandom;
	std::uniform_real_distribution<float> myLifeTimeRangeRandom;

	std::uniform_real_distribution<float> myAzimuthRandom;

	std::uniform_real_distribution<float> myForceDirectionRandomizer;

	bool myUseGravity = true;
	bool contineus;

	int id;
};
