#pragma once

#include "Vector/Vector.hpp"

struct ParticleVertexBuffer
{
	CommonUtilities::Vector4<float> color;
	CommonUtilities::Vector3<float> position;
	float size;
};

struct ParticleBuffer
{
	ParticleVertexBuffer particles[4096];

	CommonUtilities::Vector3<float> rotation;

	unsigned particleCount;
};