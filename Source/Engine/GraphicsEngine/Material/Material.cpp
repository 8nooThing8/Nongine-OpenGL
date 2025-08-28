
#ifdef OPENGL
#include "Material.h"

#include <unordered_map>

#include "../Engine/GraphicsEngine/TextureHelper.hpp"

#include <filesystem>

#include <sstream>
#include <glad/glad.h>

std::vector<Material*> Material::myMaterials;
const int Material::myTextureCount;

std::unordered_map<std::string, int> globalShaderProgram = std::unordered_map<std::string, int>();

Material::Material(const char* aFragmentShaderPath, const char* aVertexShaderPath, bool useTextures) :
	myFragmentShader(aFragmentShaderPath, ShaderCompiler::FragmentShader),
	myVertexShader(aVertexShaderPath, ShaderCompiler::VertexShader)
{
	std::string fragVertShader = std::string(aFragmentShaderPath) + std::string(aVertexShaderPath);

	if (globalShaderProgram.find(fragVertShader) != globalShaderProgram.end())
	{
		myID = globalShaderProgram[fragVertShader];
	}
	else
	{
		myID = glCreateProgram();

		myFragmentShader.Use(myID);
		myVertexShader.Use(myID);

		glLinkProgram(myID);

		glUseProgram(myID);
		if (useTextures)
		{
			SetInt("material.diffuse", DIFFUSE);
			SetInt("material.normalTexture", NORMAL);
			SetInt("material.ambientOcculusionTexture", AMBIENTOCCULUSION);
			SetInt("material.roughnessTexture", ROUGHNESS);
			SetInt("material.metallicTexture", METALLIC);
		}

		myMaterials.emplace_back(this);

		globalShaderProgram[fragVertShader] = myID;
	}
}

void Material::Use(bool useShader, bool useTextures)
{
	if (useShader)
		glUseProgram(myID);

	if (useTextures)
	{
		int index = 0;
		for (const auto& texture : myTextures)
		{
			if (texture.myID)
			{
				glActiveTexture(GL_TEXTURE0 + index);
				glBindTexture(GL_TEXTURE_2D, texture.myID);
			}

			index++;
		}
	}
}

void Material::SetMatrix4x4(const char* aName, const CommonUtilities::Matrix4x4<float>& aMatrix4x4)
{
	GLint place = glGetUniformLocation(myID, aName);
	glUniformMatrix4fv(place, 1, GL_FALSE, aMatrix4x4.myData);
}
void Material::SetMatrix3x3(const char* aName, const CommonUtilities::Matrix3x3<float>& aMatrix3x3)
{
	GLint place = glGetUniformLocation(myID, aName);
	glUniformMatrix3fv(place, 1, GL_FALSE, aMatrix3x3.myData);
}
void Material::SetVector4(const char* aName, const CommonUtilities::Vector4<float>& aVector4)
{
	GLint place = glGetUniformLocation(myID, aName);
	glUniform4f(place, aVector4.x, aVector4.y, aVector4.z, aVector4.w);
}
void Material::SetVector3(const char* aName, const CommonUtilities::Vector3<float>& aVector3)
{
	GLint place = glGetUniformLocation(myID, aName);
	glUniform3f(place, aVector3.x, aVector3.y, aVector3.z);
}
void Material::SetVector2(const char* aName, const CommonUtilities::Vector2<float>& aVector2)
{
	GLint place = glGetUniformLocation(myID, aName);
	glUniform2f(place, aVector2.x, aVector2.y);
}
void Material::SetFloat(const char* aName, float aFloat)
{
	GLint place = glGetUniformLocation(myID, aName);
	glUniform1f(place, aFloat);
}
void Material::SetInt(const char* aName, int aInt)
{
	GLint place = glGetUniformLocation(myID, aName);
	glUniform1i(place, aInt);
}
void Material::SetBool(const char* aName, float aBool)
{
	GLint place = glGetUniformLocation(myID, aName);
	glUniform1i(place, (int)aBool);
}


void Material::SetGlobalMatrix4x4(const char* aName, const CommonUtilities::Matrix4x4<float>& aMatrix4x4)
{
	GLint id;
	glGetIntegerv(GL_CURRENT_PROGRAM, &id);
	for (const auto& material : myMaterials)
	{
		glUseProgram(material->myID);
		glUniformMatrix4fv(glGetUniformLocation(material->myID, aName), 1, GL_FALSE, aMatrix4x4.myData);
	}
	glUseProgram(id);
}
void Material::SetGlobalMatrix3x3(const char* aName, const CommonUtilities::Matrix3x3<float>& aMatrix3x3)
{
	GLint id;
	glGetIntegerv(GL_CURRENT_PROGRAM, &id);
	for (const auto& material : myMaterials)
	{
		glUseProgram(material->myID);
		glUniformMatrix3fv(glGetUniformLocation(material->myID, aName), 1, false, aMatrix3x3.myData);
	}
	glUseProgram(id);
}
void Material::SetGlobalVector4(const char* aName, const CommonUtilities::Vector4<float>& aVector4)
{
	GLint id;
	glGetIntegerv(GL_CURRENT_PROGRAM, &id);
	for (const auto& material : myMaterials)
	{
		glUseProgram(material->myID);
		glUniform4f(glGetUniformLocation(material->myID, aName), aVector4.x, aVector4.y, aVector4.z, aVector4.w);
	}
	glUseProgram(id);
}
void Material::SetGlobalVector3(const char* aName, const CommonUtilities::Vector3<float>& aVector3)
{
	GLint id;
	glGetIntegerv(GL_CURRENT_PROGRAM, &id);
	for (const auto& material : myMaterials)
	{
		glUseProgram(material->myID);
		glUniform3f(glGetUniformLocation(material->myID, aName), aVector3.x, aVector3.y, aVector3.z);
	}
	glUseProgram(id);
}
void Material::SetGlobalVector2(const char* aName, const CommonUtilities::Vector2<float>& aVector2)
{
	GLint id;
	glGetIntegerv(GL_CURRENT_PROGRAM, &id);
	for (const auto& material : myMaterials)
	{
		glUseProgram(material->myID);
		glUniform2f(glGetUniformLocation(material->myID, aName), aVector2.x, aVector2.y);
	}
	glUseProgram(id);
}
void Material::SetGlobalFloat(const char* aName, float aFloat)
{
	GLint id;
	glGetIntegerv(GL_CURRENT_PROGRAM, &id);
	for (const auto& material : myMaterials)
	{
		glUseProgram(material->myID);
		glUniform1f(glGetUniformLocation(material->myID, aName), aFloat);
	}
	glUseProgram(id);
}
void Material::SetGlobalInt(const char* aName, int aInt)
{
	GLint id;
	glGetIntegerv(GL_CURRENT_PROGRAM, &id);
	for (const auto& material : myMaterials)
	{
		glUseProgram(material->myID);
		glUniform1i(glGetUniformLocation(material->myID, aName), aInt);
	}
	glUseProgram(id);
}
void Material::SetGlobalBool(const char* aName, float aBool)
{
	GLint id;
	glGetIntegerv(GL_CURRENT_PROGRAM, &id);
	for (const auto& material : myMaterials)
	{
		glUseProgram(material->myID);
		glUniform1i(glGetUniformLocation(material->myID, aName), (int)aBool);
	}
	glUseProgram(id);
}

void Material::BindTexture(unsigned aIndex, unsigned aTextureID, const char* aTextureName)
{
	if (strlen(aTextureName))
		SetInt(aTextureName, aIndex);

	glActiveTexture(GL_TEXTURE0 + aIndex);
	glBindTexture(GL_TEXTURE_2D, aTextureID);
}

unsigned Material::Get()
{
	return myID;
}

void Material::SetTexture(const char* aPath, unsigned aTextureIndex, SamplingType aSamplingType, TextureWrapMode aWrapMode)
{
	TextureHelper::CreateTexture(aPath, aSamplingType, aWrapMode, myTextures[aTextureIndex]);

	SetInt("material.diffuse", 0);
	if (aTextureIndex == 0)
	{
		std::filesystem::path path(aPath);
		myTexturePath = path.string();
		path.replace_extension();
		std::string pathString = path.filename().string();
		myTextureName = pathString;
	}
}

const char* Material::GetTextureName()
{
	return myTextureName.c_str();
}

const char* Material::GetTexturePath()
{
	return myTexturePath.c_str();
}

CU::Vector4f& Material::GetAlbedo()
{
	return myAlbedo;
}

const CU::Vector4f& Material::GetAlbedo() const
{
	return myAlbedo;
}

void Material::SetAlbedo(const CU::Vector4f& aAlbedo)
{
	myAlbedo = aAlbedo;
}
void Material::SetColor(const CU::Vector4f& aAlbedo)
{
	myAlbedo = aAlbedo;
}

Texture Material::GetTexture(unsigned aTextureIndex)
{
	return myTextures[aTextureIndex];
}

void Material::TestAndAssignDefaultMaterials()
{
	for (int i = 0; i < myTextureCount; i++)
	{
		if (myTextures[i].myID == static_cast<unsigned>(-1))
		{
			switch (i)
			{
			case 0:
				SetTexture("Assets/Textures/T_Default_C.png", DIFFUSE);
				break;
			case 1:
				SetTexture("Assets/Textures/T_Default_N.png", NORMAL);
				break;
			case 2:
				SetTexture("Assets/Textures/T_Default_M.png", AMBIENTOCCULUSION);
				break;
			case 3:
				SetTexture("Assets/Textures/T_Default_M.png", ROUGHNESS);
				break;
			case 4:
				SetTexture("Assets/Textures/T_Default_M.png", METALLIC);
				break;
			}
		}
	}
}

#elif D3D11

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
