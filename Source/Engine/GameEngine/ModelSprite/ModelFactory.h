#pragma once

#include <filesystem>

#include <map>

//#include "tgafbx.h"

#include "../Engine/GraphicsEngine/Mesh.h"

static inline std::map<const char*, non::Mesh*> globalModelsLoaded = std::map<const char*, non::Mesh*>();
//static inline std::unordered_map<std::string, TGA::FBX::Animation*> globalAnimationsLoaded = std::unordered_map<std::string, TGA::FBX::Animation*>();

static non::Mesh* ImportModelToMemory(const std::string& aCustomModelPath)
{
	std::filesystem::path customPath(aCustomModelPath.c_str());

	if (customPath.extension().string() == ".fbx")
	{
		//TGA::FBX::Mesh mesh;

		//bool succededLoad = TGA::FBX::Importer::LoadMesh(customPath, mesh);

		 non::Mesh* nonMesh = new non::Mesh();

		// std::memcpy(nonMesh->myPath.data(), mesh.Name.c_str(), mesh.Name.size() + 1);
		// nonMesh->myElements.resize(mesh.Elements.size());
		// for (int i = 0; i < mesh.Elements.size(); i++)
		// {
		// 	nonMesh->myElements[i].myVerticies.resize(mesh.Elements[i].Vertices.size());
		// 	nonMesh->myElements[i].myIndices.resize(mesh.Elements[i].Indices.size());

		// 	std::memcpy(nonMesh->myElements[i].myVerticies.data(), mesh.Elements[i].Vertices.data(), sizeof(non::Vertex) * mesh.Elements[i].Vertices.size());
		// 	std::memcpy(nonMesh->myElements[i].myIndices.data(), mesh.Elements[i].Indices.data(), sizeof(unsigned) * mesh.Elements[i].Indices.size());

		// 	std::memcpy(&nonMesh->myElements[i].myBoxBounds, &mesh.Elements[i].BoxBounds, sizeof(non::Box));
		// }

		// nonMesh->Init();

		globalModelsLoaded[aCustomModelPath.c_str()] = nonMesh;
	
		return nonMesh;
	}
	else
	{
		//Mesh customMesh(customPath);

		//globalModelsLoaded[aCustomModelPath] = customMesh;
	}

	return nullptr;
}

//static void ImportAnimationsToMemory(const std::string& aCustomModelPath)
//{
//	std::filesystem::path customPath(aCustomModelPath);
//
//	if (customPath.extension().string() == ".fbx")
//	{
//		TGA::FBX::Animation* animation = new TGA::FBX::Animation();
//
//		TGA::FBX::Importer::LoadAnimation(customPath, *animation);
//
//		if (animation->Frames.size())
//		{
//			globalAnimationsLoaded[customPath.string()] = animation;
//		}
//		else
//		{
//			//PrintError("Animation could not be loaded");
//		}
//
//	}
//	else
//	{
//		//Mesh customMesh(customPath);
//
//		//globalModelsLoaded[aCustomModelPath] = customMesh;
//	}
//}

[[maybe_unused]]
static non::Mesh* CreateModel(const std::string& aPath)
{
	if (globalModelsLoaded.find(aPath.c_str()) == globalModelsLoaded.end())
	{
		return ImportModelToMemory(aPath);
	}

	return globalModelsLoaded.at(aPath.c_str());
}

//static TGA::FBX::Animation* CreateAnimation(const std::string& aPath)
//{
//	if (!globalAnimationsLoaded.contains(aPath))
//	{
//		ImportAnimationsToMemory(aPath);
//	}
//
//	return globalAnimationsLoaded[aPath];
//}