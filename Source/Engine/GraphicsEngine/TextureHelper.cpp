#include "TextureHelper.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glad/glad.h>

#include <unordered_map>

#include <string>
#include <iostream>

std::unordered_map<std::string, Texture> globalTextures = std::unordered_map<std::string, Texture>();

bool TextureHelper::CreateTexture(const char* aTexturePath, SamplingType aSamplingType, TextureWrapMode aTextureWrapMode, Texture& outTexture)
{
	if (globalTextures.find(aTexturePath) != globalTextures.end())
	{
		outTexture = globalTextures.at(aTexturePath);
		return true;
	}

	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(aTexturePath, &outTexture.width, &outTexture.height, &outTexture.nrChannels, 0);
	glGenTextures(1, &outTexture.myID);
	glBindTexture(GL_TEXTURE_2D, outTexture.myID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, aTextureWrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, aTextureWrapMode);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, aSamplingType);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, aSamplingType);

	int rgbTypeOffset = 3 - outTexture.nrChannels;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB - rgbTypeOffset, outTexture.width, outTexture.height, 0, GL_RGB - rgbTypeOffset, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	globalTextures[aTexturePath] = outTexture;

	return true;
}
