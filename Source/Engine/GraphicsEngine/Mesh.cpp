#include "Mesh.h"

#include <glad/glad.h>

using namespace non;

void Mesh::Init()
{
	indeciesSize = static_cast<GLsizei>(sizeof(unsigned) * myElements[0].myIndices.size());

	glGenBuffers(1, &myVertexBuffer);
	glGenBuffers(1, &myIndexVertexBuffer);

	// Vertex
	{
		glBindBuffer(GL_ARRAY_BUFFER, myVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(non::Vertex) * myElements[0].myVerticies.size(), myElements[0].myVerticies.data(), GL_STATIC_DRAW);
	}

	// Index
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myIndexVertexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indeciesSize, myElements[0].myIndices.data(), GL_STATIC_DRAW);
	}

	glGenVertexArrays(1, &myVertexObjectBuffer);
	glBindVertexArray(myVertexObjectBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myIndexVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, myVertexBuffer);

	{
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(non::Vertex), (void*)0);
		glEnableVertexAttribArray(0);

		for (int i = 0; i < 4; i++)
		{
			glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, sizeof(non::Vertex), (void*)0);
			glEnableVertexAttribArray(1 + i);
		}

		for (int i = 0; i < 2; i++)
		{
			glVertexAttribPointer(5 + i, 4, GL_FLOAT, GL_FALSE, sizeof(non::Vertex), (void*)0);
			glEnableVertexAttribArray(5 + i);
		}

		glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(non::Vertex), (void*)0);
		glEnableVertexAttribArray(7);

		glVertexAttribPointer(8, 3, GL_FLOAT, GL_FALSE, sizeof(non::Vertex), (void*)0);
		glEnableVertexAttribArray(8);

		glVertexAttribPointer(9, 3, GL_FLOAT, GL_FALSE, sizeof(non::Vertex), (void*)0);
		glEnableVertexAttribArray(9);
	}

	glBindVertexArray(0);
}

void Mesh::DrawMesh()
{
	glBindVertexArray(myVertexObjectBuffer);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(myElements[0].myIndices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}