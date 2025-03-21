#pragma once

#include "Vector/Vector.hpp"

struct TrailVertex
{
	TrailVertex() = default;

	TrailVertex(const CommonUtilities::Vector4<float> aPos, const CommonUtilities::Vector2<float> aUV) : Position(aPos), UV(aUV)
	{

	}

	CommonUtilities::Vector4<float> Position = { 0, 0, 0, 0 };
	CommonUtilities::Vector2<float> UV = { 0, 0 };

	bool operator==(const TrailVertex& other) const
	{
		// A vertex is just a POD object so we can do this.
		return memcmp(this, &other, sizeof(TrailVertex)) == 0;
	}

	static inline const std::vector<VertexElementDesc> InputLayoutDefenition =
	{
		{ "SV_POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT },
	};
};