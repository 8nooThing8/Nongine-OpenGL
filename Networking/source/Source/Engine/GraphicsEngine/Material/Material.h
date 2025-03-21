#pragma once

#include "memory"

#include "string"

#include "Vector/Vector.hpp"

struct ID3D11PixelShader;
class Shader;

class Material
{
public:
	friend class Shaders;
	friend class GraphicsEngine;

	~Material() = default;
	Material() = default;

	Material(const std::wstring& vxShader, const std::wstring& pxShader, const std::wstring& aDiffuseTexture = L"", const std::wstring& aNormalTexture = L"");

	Material(const std::shared_ptr<Shader>& aPSShader, const std::shared_ptr<Shader>& aVXShader);

	void SetColor(const CommonUtilities::Vector4<float> aColor) { myAlbedoColor = aColor; };
	CommonUtilities::Vector4<float>& GetColor() { return myAlbedoColor; }
	float& GetMetallic() { return myMetalic; }
	float& GetRoughness() { return myRoughness; }
	float& GetBloomStrength() { return myBloomStrength; }
	void SetBloomStrength(const float& aBloomStrenght) { myBloomStrength = aBloomStrenght; }

	void SetShader(const std::shared_ptr<Shader>& aPSShader, const std::shared_ptr<Shader>& aVXShader, const std::shared_ptr<Shader>& aGXShader = nullptr);

	void SetShader(const std::wstring& pxShader, const std::wstring& vxShader, const std::wstring& aDiffuseTexture = L"", const std::wstring& aNormalTexture = L"");

	void SetPixelShader(const std::wstring& pxShader);
	void SetVertexShader(const std::wstring& vxShader);
	void SetGeometryShader(const std::wstring& vxShader);

	void SetDiffuseTexture(const std::wstring& aPath) const;
	void SetNormalTexture(const std::wstring& aPath) const;
	void SetRoughnessTexture(const std::wstring& aPath) const;
	void SetMetallicTexture(const std::wstring& aPath) const;
	void SetAOTexture(const std::wstring& aPath) const;


	void SetDiffuseTextureToNull() const;
	void SetNormalTextureToNull() const;
	void SetRoughnessTextureToNull() const;
	void SetMetallicTextureToNull() const;
	void SetAOTextureToNull() const;

	float& GetAOStrength() { return myAOStrenght; }
	float& GetNormalStrength() { return myNormalStrenght; }
	void SetAOStrength(const float aAOStrength) { myAOStrenght = aAOStrength; }
	void SetNormalStrength(const float aNormalStrength) { myNormalStrenght = aNormalStrength; }

	std::shared_ptr<Shader>& GetPSShader() { return myPSShader; }
	std::shared_ptr<Shader>& GetVXShader() { return myVXShader; }
	std::shared_ptr<Shader>& GetGXShader() { return myGXShader; }

	std::wstring shaderNamePX;

	bool isTransparant;

private:
	std::shared_ptr<Shader> myPSShader;
	std::shared_ptr<Shader> myVXShader;
	std::shared_ptr<Shader> myGXShader;

	CommonUtilities::Vector4<float> myAlbedoColor;
	float myMetalic;
	float myRoughness;
	float myBloomStrength;

	float myAOStrenght;
	float myNormalStrenght;
};