#pragma once

#include "Texture.h"

class TextureHelper
{
public:
	static bool CreateTexture(const char* aTexturePath, SamplingType aSamplingType, TextureWrapMode aTextureWrapMode, Texture& outTexture);
};
