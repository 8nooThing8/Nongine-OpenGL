#pragma once

#include <Vector/Vector.hpp>

#include <Matrix/Matrix4x4.hpp>

#define MAX_INSTANCES 100000

typedef int GLsizei;

namespace non
{
	struct SpriteVertex
	{
		CommonUtilities::Vector2f position;
		CommonUtilities::Vector2f uv;
	};

	struct InstanceVertex
	{
		CU::Matrix4x4<float> transform;
		CU::Vector4f pixelPick;
	};

	class Sprite
	{
	public:
		Sprite() = default;
		~Sprite() = default;
		
		void Init();

		void DrawSprite() const;
		void DrawSpriteInstanced(int aInstanceCount) const;

		unsigned myVertexObjectBuffer;
		static inline unsigned myInstanceVertexBuffer;

		static inline InstanceVertex transformMatricies[MAX_INSTANCES];

	private:
		unsigned myVertexBuffer;
		unsigned myIndexVertexBuffer;
	};
}
