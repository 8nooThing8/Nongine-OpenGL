#pragma once

#include <Timer.h>
#include <Vector\Vector.hpp>

#include <random>

#include "TrailVertex.h"

#include "../../Camera.h"
#include "../engine/GameEngine/InspectorCamera.h"

namespace CU = CommonUtilities;

struct TrailPoint
{
public:
	TrailPoint()
	{
		
	}
	TrailPoint(const TrailPoint& aParticle)
	{
		LifeTime = aParticle.LifeTime;
		position = aParticle.position;
		rotation = aParticle.rotation;
		thickness = aParticle.thickness;
		linearVelocity = aParticle.linearVelocity;
		//angularVelocity = aParticle.angularVelocity;
		myColor = aParticle.myColor;
		timeAlive = aParticle.timeAlive;
	}

	bool Update()
	{
		float deltaTime = CommonUtilities::Time::GetDeltaTime();

		timeAlive += deltaTime;

		if (timeAlive >= LifeTime)
			return true;

		return false;
	}

	float LifeTime;
	CommonUtilities::Vector4<float> position;
	CommonUtilities::Vector4<float> rotation;
	float thickness;

	CommonUtilities::Vector4<float> linearVelocity;
	//CommonUtilities::Vector4<float> angularVelocity;
	CommonUtilities::Vector4<float> myColor;

	friend class Trail;

private:
	float timeAlive = 0;
};