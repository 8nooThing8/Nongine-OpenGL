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

Material::Material(const std::shared_ptr<Shader>& aPSShader, const std::shared_ptr<Shader>& aVXShader):
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
