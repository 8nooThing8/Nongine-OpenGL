#pragma once

#include <Timer.h>
#include <Vector\Vector4.hpp>

#include <random>

struct Particle
{
	Particle()
	{
		
	}
	Particle(const Particle& aParticle)
	{
		LifeTime = aParticle.LifeTime;
		position = aParticle.position;
		//rotation = aParticle.rotation;
		scale = aParticle.scale;
		linearVelocity = aParticle.linearVelocity;
		//angularVelocity = aParticle.angularVelocity;
		myColor = aParticle.myColor;
		timeAlive = aParticle.timeAlive;

		linearDrag = aParticle.linearDrag;
		angularDrag = aParticle.angularDrag;

		useGravity = aParticle.useGravity;

		direction = aParticle.direction;
	}

	bool Update()
	{
		float deltaTime = CommonUtilities::Time::GetDeltaTime();

		//rotation += angularVelocity * deltaTime;

		linearVelocity -= CommonUtilities::Vector4<float>(linearDrag, linearDrag + (static_cast<int>(useGravity) * 9.81f) * 25, linearDrag, 0) * deltaTime;

		linearVelocity.x = CommonUtilities::Max(linearVelocity.x, 0.f);
		linearVelocity.z = CommonUtilities::Max(linearVelocity.z, 0.f);

		//position.y += (static_cast<int>(useGravity) * 9.81f) * deltaTime;

		position += (linearVelocity * CommonUtilities::Vector4<float>(static_cast<float>(direction.x), 1.f, static_cast<float>(direction.z), 0.f)) * deltaTime;




		timeAlive += deltaTime;

		if (timeAlive >= LifeTime)
			return true;

		return false;
	}


	float LifeTime;
	CommonUtilities::Vector4<float> position;
	//CommonUtilities::Vector4<float> rotation;
	CommonUtilities::Vector4<float> scale;

	CommonUtilities::Vector4<float> linearVelocity;
	//CommonUtilities::Vector4<float> angularVelocity;
	std::vector<CommonUtilities::Vector4<float>> myColor;

	CommonUtilities::Vector4<float> myCurrentColor;

	CommonUtilities::Vector3<int> direction;

	float linearDrag;
	float angularDrag;

	bool useGravity = false;

private:
	float timeAlive = 0;
};