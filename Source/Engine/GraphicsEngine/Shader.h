#pragma once

#include "ShaderCompiler.h"

#include <String.hpp>

class Shader
{
public:
	Shader() = default;
	~Shader() = default;
	Shader(const char* aPath, ShaderCompiler::ShaderType aType);

	unsigned GetID();

	void Use(unsigned aProgramID);

	void operator =(const Shader& aShader) 
	{
		myPath = aShader.myPath;
		myID = aShader.myID;
		isNull = aShader.isNull;
	}

private:
	std::string myPath;
	unsigned myID;

	bool isNull = false;
};