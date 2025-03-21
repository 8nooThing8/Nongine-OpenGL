#pragma once

#include <string>
#include <unordered_map>

#include "tgafbx.h"

#include "d3d11.h"
#include "../Engine/GameEngine/Console.h"

static inline std::unordered_map<std::string, TGA::FBX::Mesh> globalModelsLoaded = std::unordered_map<std::string, TGA::FBX::Mesh>();
static inline std::unordered_map<std::string, TGA::FBX::Animation> globalAnimationsLoaded = std::unordered_map<std::string, TGA::FBX::Animation>();

static void ImportModelToMemory(const std::string& aCustomModelPath)
{
	std::filesystem::path customPath(aCustomModelPath);

	if (customPath.extension().string() == ".fbx")
	{
		TGA::FBX::Mesh mesh;

		bool succededLoad = TGA::FBX::Importer::LoadMesh(customPath, mesh);

		if (!succededLoad)
		{
			PrintError("Something went wrong loading a model");
		}

		if (mesh.Elements.size())
		{
			globalModelsLoaded[customPath.string()] = mesh;
		}
		else
		{
			PrintError("Model does not have any verticies");
		}
	}
	else
	{
		//Mesh customMesh(customPath);

		//globalModelsLoaded[aCustomModelPath] = customMesh;
	}
}

static void ImportAnimationsToMemory(const std::string& aCustomModelPath)
{
	std::filesystem::path customPath(aCustomModelPath);

	if (customPath.extension().string() == ".fbx")
	{
		TGA::FBX::Animation animation;

		TGA::FBX::Importer::LoadAnimation(customPath, animation);

		if (animation.Frames.size())
		{
			globalAnimationsLoaded[customPath.string()] = animation;
		}
		else
		{
			PrintError("Animation could not be loaded");
		}

	}
	else
	{
		//Mesh customMesh(customPath);

		//globalModelsLoaded[aCustomModelPath] = customMesh;
	}
}

static TGA::FBX::Mesh* CreateModel(const std::string& aPath)
{
	if (!globalModelsLoaded.contains(aPath))
	{
		ImportModelToMemory(aPath);
	}

	return new TGA::FBX::Mesh(globalModelsLoaded[aPath]);
}

static TGA::FBX::Animation* CreateAnimation(const std::string& aPath)
{
	if (!globalAnimationsLoaded.contains(aPath))
	{
		ImportAnimationsToMemory(aPath);
	}

	return new TGA::FBX::Animation(globalAnimationsLoaded[aPath]);
}