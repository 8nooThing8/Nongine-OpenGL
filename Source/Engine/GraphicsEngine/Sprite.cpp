#include "Sprite.h"

#include <glad/glad.h>

using namespace non;

// 4 vertices
static SpriteVertex vertices[]
{
	{{ 0, 0 }, {0, 0}},
	{{ 1.f, 0 }, {1, 0}},
	{{ 0, 1.f }, {0, 1}},
	{{ 1.f, 1.f }, {1, 1}},
};

// 6 indices
static unsigned indices[]
{
	0, 1, 2,
	2, 1, 3
};

void non::Sprite::Init()
{
	glGenBuffers(1, &myInstanceVertexBuffer);

	glGenBuffers(1, &myVertexBuffer);
	glGenBuffers(1, &myIndexVertexBuffer);

	// Vertex
	{
		glBindBuffer(GL_ARRAY_BUFFER, myVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(SpriteVertex) * 4, vertices, GL_STATIC_DRAW);
	}

	// Index
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myIndexVertexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * 4, indices, GL_STATIC_DRAW);
	}

	glGenVertexArrays(1, &myVertexObjectBuffer);
	glBindVertexArray(myVertexObjectBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, myVertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myIndexVertexBuffer);

	{
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (void*)(2 * sizeof(float)));
	}

	glBindBuffer(GL_ARRAY_BUFFER, myInstanceVertexBuffer);

	const int vec4Size = sizeof(CommonUtilities::Vector4f);

	const int instanceStride = sizeof(InstanceVertex);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, instanceStride, (void*)(0 * vec4Size));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, instanceStride, (void*)(1 * vec4Size));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, instanceStride, (void*)(2 * vec4Size));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, instanceStride, (void*)(3 * vec4Size));

	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, instanceStride, (void*)(4 * vec4Size));

	// glVertexAttribDivisor(2, 1);
	// glVertexAttribDivisor(3, 1);
	// glVertexAttribDivisor(4, 1);
	// glVertexAttribDivisor(5, 1);
	// glVertexAttribDivisor(6, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Sprite::DrawSprite() const
{
	glBindVertexArray(myVertexObjectBuffer);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Sprite::DrawSpriteInstanced(int aInstanceCount) const
{
	glBindBuffer(GL_ARRAY_BUFFER, non::Sprite::myInstanceVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(InstanceVertex) * aInstanceCount, non::Sprite::transformMatricies->transform.myData, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(myVertexObjectBuffer);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, aInstanceCount);
	glBindVertexArray(0);
}