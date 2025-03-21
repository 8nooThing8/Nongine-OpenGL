
#ifdef OPENGL
#include "Material.h"
#include <glad\glad.h>

Material::Material(const char* aFragmentShaderPath, const char* aVertexShaderPath) : 
	myFragmentShader(aFragmentShaderPath, ShaderCompiler::FragmentShader), 
	myVertexShader(aVertexShaderPath, ShaderCompiler::VertexShader),
	myID(glCreateProgram())
{
	myFragmentShader.Use(myID);
	myVertexShader.Use(myID);

	glLinkProgram(myID);
}

void Material::Use()
{
	glUseProgram(myID);
}

void Material::SetMatrix4x4(const std::string& aName, const CommonUtilities::Matrix4x4<float>& aMatrix4x4)
{
	glUniformMatrix4fv(glGetUniformLocation(myID, aName.c_str()), 1, GL_FALSE, aMatrix4x4.myData);
}

//void Material::SetMatrix3x3(const std::string& aName, const CommonUtilities::Matrix3x3<float>& aMatrix3x3)
//{
//	glUniformMatrix3fv(glGetUniformLocation(myID, aName.c_str()), 16, false, aMatrix3x3.myData);
//}

void Material::SetVector4(const std::string& aName, const CommonUtilities::Vector4<float>& aVector4)
{
	glUniform4f(glGetUniformLocation(myID, aName.c_str()), aVector4.x, aVector4.y, aVector4.z, aVector4.w);
}
void Material::SetVector3(const std::string& aName, const CommonUtilities::Vector3<float>& aVector3)
{
	glUniform3f(glGetUniformLocation(myID, aName.c_str()), aVector3.x, aVector3.y, aVector3.z);
}
void Material::SetVector2(const std::string& aName, const CommonUtilities::Vector2<float>& aVector2)
{
	glUniform2f(glGetUniformLocation(myID, aName.c_str()), aVector2.x, aVector2.y);
}
void Material::SetFloat(const std::string& aName, float aFloat)
{
	glUniform1f(glGetUniformLocation(myID, aName.c_str()), aFloat);
}
void Material::SetInt(const std::string& aName, int aInt)
{
	glUniform1i(glGetUniformLocation(myID, aName.c_str()), aInt);
}
void Material::SetBool(const std::string& aName, float aBool)
{
	glUniform1i(glGetUniformLocation(myID, aName.c_str()), (int)aBool);
}


unsigned Material::Get()
{
	return myID;
}

#elif D3D11

#include "../../Engine.pch.h"
#include "Material.h"

#include "../Shader.h"

#include "../ShaderCompiler.h"

#include "../../GameEngine/Console.h"


#include <wrl.h>
#include <filesystem>

#include <d3dcompiler.h>


Material::Material(const std::wstring& pxShader, const std::wstring& vxShader, const std::wstring& aDiffuseTexture, const std::wstring& aNormalTexture)
{
	SetShader(pxShader, vxShader, aDiffuseTexture, aNormalTexture);

	myMetalic = 0;
	myRoughness = 0;

	myNormalStrenght = 1;
	myAOStrenght = 1;
	myBloomStrength = 0;
}

Material::Material(const std::shared_ptr<Shader>& aPSShader, const std::shared_ptr<Shader>& aVXShader) :
	isTransparant(false),
	myMetalic(0),
	myRoughness(0),
	myBloomStrength(0),
	myAOStrenght(0),
	myNormalStrenght(0)
{
	myPSShader = aPSShader;
	myVXShader = aVXShader;
}

void Material::SetShader(const std::shared_ptr<Shader>& aPSShader, const std::shared_ptr<Shader>& aVXShader, const std::shared_ptr<Shader>& aGXShader)
{
	myPSShader = aPSShader;
	myVXShader = aVXShader;
	myGXShader = aGXShader;
}

void Material::SetShader(const std::wstring& pxShader, const std::wstring& vxShader, const std::wstring& aDiffuseTexture, const std::wstring& aNormalTexture)
{
	myPSShader = std::make_shared<Shader>(aDiffuseTexture, aNormalTexture);
	myVXShader = std::make_shared<Shader>(L"", L"");

	myPSShader->SetShaderNamePath(pxShader, pxShader);
	myVXShader->SetShaderNamePath(vxShader, vxShader);

	Shaders::CompileShader(this);

	size_t lastSlashPos = aDiffuseTexture.find_last_of(L"/\\");

	std::wstring filenameWithExtension;
	if (lastSlashPos != std::wstring::npos)
		filenameWithExtension = aDiffuseTexture.substr(lastSlashPos + 1);
	else
		filenameWithExtension = aDiffuseTexture;

	size_t lastDotPos = filenameWithExtension.find_last_of(L".");

	std::wstring fileName;
	if (lastDotPos != std::wstring::npos)
		fileName = filenameWithExtension.substr(0, lastDotPos);
	else
		fileName = filenameWithExtension;

	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &fileName[0], (int)fileName.size(), NULL, 0, NULL, NULL);
	std::string name(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &fileName[0], (int)fileName.size(), &name[0], size_needed, NULL, NULL);

	myPSShader->SetDiffuseName(name);

	lastSlashPos = aNormalTexture.find_last_of(L"/\\");

	if (lastSlashPos != std::wstring::npos)
		filenameWithExtension = aNormalTexture.substr(lastSlashPos + 1);
	else
		filenameWithExtension = aNormalTexture;

	lastDotPos = filenameWithExtension.find_last_of(L".");


	if (lastDotPos != std::wstring::npos)
		fileName = filenameWithExtension.substr(0, lastDotPos);
	else
		fileName = filenameWithExtension;

	size_needed = WideCharToMultiByte(CP_UTF8, 0, &fileName[0], (int)fileName.size(), NULL, 0, NULL, NULL);
	name = std::string(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &fileName[0], (int)fileName.size(), &name[0], size_needed, NULL, NULL);

	myPSShader->SetNormalName(name);

	std::string diffuseName = std::filesystem::path(aDiffuseTexture).string();
	std::string normalName = std::filesystem::path(aNormalTexture).string();

	myPSShader->SetDiffuseName(diffuseName);
	myPSShader->SetNormalName(normalName);
}

void Material::SetPixelShader(const std::wstring& pxShader)
{
	if (pxShader.empty())
	{
		myPSShader = nullptr;
		return;
	}

	myPSShader = std::make_shared<Shader>(L"", L"");

	myPSShader->SetShaderNamePath(pxShader, pxShader);

	Shaders::CompileShader(this);
}

void Material::SetVertexShader(const std::wstring& vxShader)
{
	myVXShader = std::make_shared<Shader>(L"", L"");

	myVXShader->SetShaderNamePath(vxShader, vxShader);

	Shaders::CompileShader(this);
}

void Material::SetGeometryShader(const std::wstring& vxShader)
{
	myGXShader = std::make_shared<Shader>(L"", L"");

	myGXShader->SetShaderNamePath(vxShader, vxShader);

	Shaders::CompileShader(this);
}

void Material::SetDiffuseTexture(const std::wstring& aPath) const
{
	myPSShader->SetDiffuseTexture(aPath);
}

void Material::SetNormalTexture(const std::wstring& aPath) const
{
	myPSShader->SetNormalTexture(aPath);
}

void Material::SetRoughnessTexture(const std::wstring& aPath) const
{
	myPSShader->SetRoughnessTexture(aPath);
}

void Material::SetMetallicTexture(const std::wstring& aPath) const
{
	myPSShader->SetMetallicTexture(aPath);
}

void Material::SetAOTexture(const std::wstring& aPath) const
{
	myPSShader->SetAOTexture(aPath);
}


void Material::SetDiffuseTextureToNull() const
{
	myPSShader->SetDiffuseTextureToNull();
}

void Material::SetNormalTextureToNull() const
{
	myPSShader->SetNormalTextureToNull();
}

void Material::SetRoughnessTextureToNull() const
{
	myPSShader->SetRoughnessTextureToNull();
}

void Material::SetMetallicTextureToNull() const
{
	myPSShader->SetMetallicTextureToNull();
}

void Material::SetAOTextureToNull() const
{
	myPSShader->SetAOTextureToNull();
}

#endif
