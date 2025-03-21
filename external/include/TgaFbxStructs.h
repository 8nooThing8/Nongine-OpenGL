#pragma once
#include <array>
#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>
#include <xmmintrin.h>

#include "Matrix\Matrix.hpp"

#pragma warning( push )
#pragma warning( disable : 4201 )

namespace TGA
{
	namespace FBX
	{
		struct FBXMaterial;

		// Extremely barebones container for a 4x4 float matrix.
		struct Matrix
		{
			union
			{
				float Data[16];
				struct
				{
					__m128 m1;
					__m128 m2;
					__m128 m3;
					__m128 m4;
				};
				struct
				{
					float m11;
					float m21;
					float m31;
					float m41;

					float m12;
					float m22;
					float m32;
					float m42;

					float m13;
					float m23;
					float m33;
					float m43;

					float m14;
					float m24;
					float m34;
					float m44;
				};
			};

			Matrix()
				: Data{ 1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						0, 0, 0, 1 }
			{}

			const float& operator()(const int aRow, const int aColumn) const;
			float& operator()(const int aRow, const int aColumn);
			float& operator[](const unsigned int& aIndex);
			const float& operator[](const unsigned int& aIndex) const;
			Matrix operator*(const Matrix& aRightMatrix) const;
			Matrix& operator*=(const Matrix& aMatrix);

			Matrix operator-(const Matrix& aRightMatrix)
			{
				Matrix newMatrix;

				for (int i = 1; i < 5; i++)
				{
					for (int j = 1; j < 5; j++)
					{
						newMatrix(j, i) = this->operator()(j, i) - aRightMatrix(j, i);
					}
				}

				return newMatrix;
			}

			Matrix operator+(const Matrix& aRightMatrix)
			{
				Matrix newMatrix;

				for (int i = 1; i < 5; i++)
				{
					for (int j = 1; j < 5; j++)
					{
						newMatrix(j, i) = this->operator()(j, i) + aRightMatrix(j, i);
					}
				}

				return newMatrix;
			}

			Matrix operator*(const float number)
			{
				Matrix newMatrix;

				for (int i = 1; i < 5; i++)
				{
					for (int j = 1; j < 5; j++)
					{
						newMatrix(j, i) = this->operator()(j, i) * number;
					}
				}

				return newMatrix;
			}

			Matrix GetTranspose() const
			{
				Matrix newMatrix;

				for (int i = 1; i < 5; i++)
				{
					for (int j = 1; j < 5; j++)
					{
						newMatrix(j, i) = this->operator()(i, j);
					}
				}

				return newMatrix;
			}

		};

		struct Box
		{
			float Min[3]{ 0, 0, 0 };
			float Max[3]{ 0, 0, 0 };
			bool IsValid = false;

			Box& operator+=(const std::array<float, 3> aVector);
			static Box FromAABB(const std::array<float, 3> anOrigin, const std::array<float, 3> anExtent);
		};

		struct BoxSphereBounds
		{
			std::array<float, 3> BoxExtents = { 0, 0, 0 };
			std::array<float, 3> Center = { 0, 0, 0 };
			float Radius{ 0 };
			BoxSphereBounds operator+(const BoxSphereBounds& aBounds) const;
		};

		struct Vertex
		{
			float Position[4] = { 0,0,0,1 };
			float VertexColors[4][4]
			{
				{0, 0, 0, 0},
				{0, 0, 0, 0},
				{0, 0, 0, 0},
				{0, 0, 0, 0},
			};

			float UVs[4][2]
			{
				{0, 0},
				{0, 0},
				{0, 0},
				{0, 0}
			};

			float Normal[3] = { 0, 0, 0 };
			float Tangent[3] = { 0, 0, 0 };
			float BiNormal[3] = { 0, 0, 0 };

			unsigned int BoneIDs[4] = { 0, 0, 0, 0 };
			float BoneWeights[4] = { 0, 0, 0, 0 };

			

			bool operator==(const Vertex& other) const
			{
				// A vertex is just a POD object so we can do this.
				return memcmp(this, &other, sizeof(Vertex)) == 0;
			}

			/*static inline const std::vector<VertexElementDesc> InputLayoutDefenition =
			{
				{ "SV_POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT },

				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT },
				{ "COLOR", 1, DXGI_FORMAT_R32G32B32A32_FLOAT },
				{ "COLOR", 2, DXGI_FORMAT_R32G32B32A32_FLOAT },
				{ "COLOR", 3, DXGI_FORMAT_R32G32B32A32_FLOAT },

				{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT },
				{ "UV", 1, DXGI_FORMAT_R32G32_FLOAT },
				{ "UV", 2, DXGI_FORMAT_R32G32_FLOAT },
				{ "UV", 3, DXGI_FORMAT_R32G32_FLOAT },

				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT },

				{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT },

				{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT },

				{ "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT },

				{ "BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT },

				{ "RELATIVEXF", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, true },
				{ "RELATIVEXF", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, true },
				{ "RELATIVEXF", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, true },
				{ "RELATIVEXF", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, true },
			};*/
		};

		struct Texture
		{
			std::string Name;
			std::string Path;
			std::string RelativePath;
		};

		struct Material
		{
			std::string MaterialName;
			Texture Emissive;
			Texture Ambient;
			Texture Diffuse;
			Texture Specular;
			Texture Shininess;
			Texture Bump;
			Texture NormalMap;
			Texture TransparentColor;
			Texture Reflection;
			Texture Displacement;
			Texture VectorDisplacement;
		};

		enum class Axis : uint8_t
		{
			X = 0,
			Y = 1,
			Z = 2
		};

		enum class SystemUnit : uint8_t
		{
			Unknown,
			Millimeter,
			Centimeter,
			Decimeter,
			Meter,
			Inch,
			Foot,
			Mile,
			Yard
		};

		enum class FbxImportWarning : uint8_t
		{
			TooManySkinWeights,
			NoUVCoordinates,
			GenerateNormalsFailed,
			CannotTriangulate,
			DuplicateBoneNames,
		};

		enum class FbxImportResult : uint8_t
		{
			Success,
			SuccessWithWarnings,
			InternalError,
			NotInitialized,
			FileNotFound,
			NoSkeletonData,
			NoAnimationData,
			NoMeshData,
		};

		struct FbxImportStatus
		{
			FbxImportResult Result;
			std::unordered_map<FbxImportWarning, uint32_t> Warnings;

			operator bool() const
			{
				return Result == FbxImportResult::Success || Result == FbxImportResult::SuccessWithWarnings;
			}

		private:

			friend class Importer;
			friend class Internals;

			void AddWarning(FbxImportWarning aWarning)
			{
				if (Warnings.find(aWarning) != Warnings.end())
				{
					Warnings.at(aWarning)++;
				}
				else
				{
					Warnings.emplace(aWarning, 1);
				}
			}
		};

		struct FileInfo
		{
			std::string Application;
			std::string ApplicationVersion;
			// The original Up axis of the file before conversion to DirectX Left Handed Y-Up.
			Axis OriginalUpAxis;
			// The system unit in the file before conversion to Centimeter.
			SystemUnit OriginalSystemUnit = SystemUnit::Unknown;
		};

		struct Skeleton
		{
			std::string Name;

			struct Bone
			{
				Matrix BindPoseInverse;
				int ParentIdx = -1;
				std::string NamespaceName;
				std::string Name;
				std::vector<unsigned> Children;
			};

			struct Socket
			{
				Matrix RestTransform;
				int ParentBoneIdx = -1;
				std::string Name;
				std::string NamespaceName;
			};

			std::vector<Bone> Bones;
			std::unordered_map<std::string, Socket> Sockets;
			std::unordered_map<std::string, size_t> BoneNameToIndex;

			const Bone* GetRoot() const { if (!Bones.empty()) { return &Bones[0]; } return nullptr; }
		};

		struct Mesh
		{
			Mesh() = default;

			Mesh(Mesh& aMesh) :
				FileInfo(aMesh.FileInfo),
				Skeleton(aMesh.Skeleton),
				Elements(aMesh.Elements),
				Materials(aMesh.Materials),
				LODGroups(aMesh.LODGroups),
				Name(aMesh.Name),
				BoxSphereBounds(aMesh.BoxSphereBounds),
				BoxBounds(aMesh.BoxBounds)
			{
				
			}

			/*void Init()
			{
				std::vector<TGA::FBX::Vertex> verticies;
				std::vector<unsigned> indicies;

				for (auto& element : Elements)
				{
					verticies.insert(verticies.end(), element.Vertices.begin(), element.Vertices.end());
					indicies.insert(indicies.end(), element.Indices.begin(), element.Indices.end());
				}

				GraphicsEngine::GetRHI()->CreateVertexBuffer("VertexBuffer", verticies, myVertexBuffer);
				GraphicsEngine::GetRHI()->CreateIndexBuffer("IndexBuffer", indicies, myIndexBuffer);
			}*/

			struct Element
			{
				std::vector<Vertex> Vertices;
				std::vector<unsigned int> Indices;

				unsigned int MaterialIndex;
				std::string MeshName;
				BoxSphereBounds BoxSphereBounds;
				Box BoxBounds;
			};

			struct LODGroup
			{
				struct LODLevel
				{
					unsigned int Level;
					float Distance;
					std::vector<Element> Elements;
					BoxSphereBounds BoxSphereBounds;
				};

				std::vector<LODLevel> Levels;
			};

			FileInfo FileInfo;

			Skeleton Skeleton;

			std::vector<Element> Elements;
			std::vector<Material> Materials;
			std::vector<LODGroup> LODGroups;

			std::string Name;

			BoxSphereBounds BoxSphereBounds;
			Box BoxBounds;

			//Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
			//Microsoft::WRL::ComPtr<ID3D11Buffer> myIndexBuffer;

			__forceinline bool IsValid() const
			{
				return (!Elements.empty() || !LODGroups.empty());
			}
		};

		struct NavMesh
		{
			struct NavMeshPolygon
			{
				std::vector<unsigned int> Indices;
			};

			struct NavMeshChunk
			{
				std::vector<Vertex> Vertices;
				std::vector<NavMeshPolygon> Polygons;

				std::string ChunkName;
			};

			std::vector<NavMeshChunk> Chunks;
			std::string Name;
			BoxSphereBounds BoxSphereBounds;
		};

		struct Animation
		{
			struct Frame
			{
				// Stores Joint Name to Transform.
				std::unordered_map<std::string, Matrix> GlobalTransforms;
				// Stores Joint Name to Transform for Bone Space Transforms.
				std::unordered_map<std::string, Matrix> LocalTransforms;
				// A list of events that are triggered this frame.
				// Lets you use .find to see if it's here or not instead of
				// looping.
				std::unordered_map<std::string, bool> TriggeredEvents;

				std::unordered_map<std::string, Matrix> SocketTransforms;
			};

			FileInfo FileInfo;

			// The animation frames.
			std::vector<Frame> Frames;

			// A list of events that exist in this animation.
			std::vector<std::string> EventNames;

			// How long this animation is in frames.
			unsigned int Length;

			// The duration of this animation.
			double Duration;

			// The FPS of this animation.
			float FramesPerSecond;

			std::string Name;
			std::string Path;
		};
	}
}

#pragma warning( pop )