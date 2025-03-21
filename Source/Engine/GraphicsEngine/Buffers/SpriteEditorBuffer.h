#pragma once

#include "Vector\Vector.hpp"

struct SpriteEditorBuffer
{
	CommonUtilities::Vector4<float> spritesMinMax[256];

	CommonUtilities::Vector2<float> textureResolution;

	int numberOfSprites;

	float zoomAmount;

	int selectedSprite;
	int hoveredSprite;

	CommonUtilities::Vector2<float> trash;
};