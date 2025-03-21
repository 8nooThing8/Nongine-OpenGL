#pragma once

#include <Vector\Vector.hpp>

namespace non
{
	struct Vertex
	{
		CommonUtilities::Vector4f vertexPosition;
		CommonUtilities::Vector4f vertexColor;
		CommonUtilities::Vector2f UV;
	};

	struct Element
	{
		std::vector<Vertex> myVerticies;
	};

	struct Mesh
	{
		std::vector<Element> myElement;
	};
}