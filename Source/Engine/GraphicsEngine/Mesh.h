#pragma once

#include "Vertex.h"
#include <Matrix/Matrix4x4.hpp>
#include <map>
#include <String.hpp>

typedef int GLsizei;

namespace non
{
	struct Skeleton
	{
		std::string Name;

		struct Bone
		{
			CU::Matrix4x4<float> BindPoseInverse;
			int ParentIdx = -1;
			std::string NamespaceName;
			std::string Name;
			std::vector<unsigned> Children;
		};

		struct Socket
		{
			CU::Matrix4x4<float> RestTransform;
			int ParentBoneIdx = -1;
			std::string Name;
			std::string NamespaceName;
		};

		std::vector<Bone> Bones;
		std::map<const char*, Socket> Sockets;
		std::map<const char*, size_t> BoneNameToIndex;

		const Bone* GetRoot() const { if (!Bones.empty()) { return &Bones[0]; } return nullptr; }
	};

	struct Box
	{
		CU::Vector3f Min{ 0, 0, 0 };
		CU::Vector3f Max{ 0, 0, 0 };
		bool IsValid = false;
	};

	struct BoxSphereBounds
	{
		CU::Vector3f BoxExtents = { 0, 0, 0 };
		CU::Vector3f Center = { 0, 0, 0 };
		float Radius{ 0 };
	};

	struct Element
	{
		std::vector<Vertex> myVerticies;
		std::vector<unsigned int> myIndices;
		unsigned int myMaterialIndex;
		std::string myMeshName;
		BoxSphereBounds myBoxSphereBounds;
		Box myBoxBounds;
	};

	struct LODGroup
	{
		struct LODLevel
		{
			unsigned int Level;
			float Distance;
			std::vector<Element> myElements;
		};

		std::vector<LODLevel> Levels;
	};

	class Mesh
	{
	public:
		Mesh() = default;
		Mesh(Mesh& aMesh) :
			mySkeleton(aMesh.mySkeleton),
			myElements(aMesh.myElements),
			myLODGroups(aMesh.myLODGroups),
			myPath(aMesh.myPath)
		{

		}

		void Init();
		void DrawMesh();

		std::string myPath;

		Skeleton mySkeleton;

		std::vector<Element> myElements;
		std::vector<LODGroup> myLODGroups;

		unsigned myVertexObjectBuffer;

	private:
		unsigned myVertexBuffer;
		unsigned myIndexVertexBuffer;

		GLsizei indeciesSize;
	};
}