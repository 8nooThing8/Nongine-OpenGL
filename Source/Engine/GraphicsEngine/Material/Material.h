#pragma once

#include "Material/Material.h"

#include <Vector/Vector.hpp>
#include <Matrix/Matrix4x4.hpp>
#include <Matrix/Matrix3x3.hpp>

#include "../Shader.h"

#include "Texture.h"

#define ALBEDO 0
#define DIFFUSE 0
#define NORMAL 1
#define AMBIENTOCCULUSION 2
#define ROUGHNESS 3
#define METALLIC 4
#define CUSTOMTEXTURESTART 5

class Material
{
public:
	~Material() = default;
	Material() = default;
	
	Material(const char* aFragmentShaderPath, const char* aVertexShaderPath, bool useTextures = true);
	void Use(bool useShader = true, bool useTextures = true);
	unsigned Get();

	void SetMatrix4x4(const char* aName, const CommonUtilities::Matrix4x4<float>& aMatrix);
	void SetMatrix3x3(const char* aName, const CommonUtilities::Matrix3x3<float>& aMatrix);
	void SetVector4(const char* aName, const CommonUtilities::Vector4<float>& aVector4);
	void SetVector3(const char* aName, const CommonUtilities::Vector3<float>& aVector3);
	void SetVector2(const char* aName, const CommonUtilities::Vector2<float>& aVector2);
	void SetFloat(const char* aName, float aFloat);
	void SetInt(const char* aName, int aInt);
	void SetBool(const char* aName, float aBool);

	// Only need to provide textre name if it is one of the original 5 that exists (DIFFUSE, NORMAL, AMBIENTOCCULUSION, ROUGHNESS or METALLIC)
	void BindTexture(unsigned aIndex, unsigned aTextureID, const char* aTextureName = "");

	static void SetGlobalMatrix4x4(const char* aName, const CommonUtilities::Matrix4x4<float>& aMatrix);
	static void SetGlobalMatrix3x3(const char* aName, const CommonUtilities::Matrix3x3<float>& aMatrix);
	static void SetGlobalVector4(const char* aName, const CommonUtilities::Vector4<float>& aVector4);
	static void SetGlobalVector3(const char* aName, const CommonUtilities::Vector3<float>& aVector3);
	static void SetGlobalVector2(const char* aName, const CommonUtilities::Vector2<float>& aVector2);
	static void SetGlobalFloat(const char* aName, float aFloat);
	static void SetGlobalInt(const char* aName, int aInt);
	static void SetGlobalBool(const char* aName, float aBool);

	float& GetSpecular() { return mySpecular; };
	float& GetSpecularStrength() { return mySpecularStrength; };

	void SetSpecular(float aSpecular) { mySpecular = aSpecular; };
	void SetSpecularStrength(float aSpecStrenght) { mySpecularStrength = aSpecStrenght; };

	void SetTexture(const char* aPath, unsigned aTextureIndex, SamplingType aSamplingType = Bilinear, TextureWrapMode aWrapMode = Repeat);

	const char* GetTextureName();
	const char* GetTexturePath();

	CU::Vector4f& GetAlbedo();
	const CU::Vector4f& GetAlbedo() const;
	void SetAlbedo(const CU::Vector4f& aAlbedo);
	void SetColor(const CU::Vector4f& aAlbedo);

	Texture GetTexture(unsigned aTextureIndex);

	void TestAndAssignDefaultMaterials();

	static std::vector<Material*> myMaterials;

private:
	std::string myTextureName;
	std::string myTexturePath;

	static const int myTextureCount = 5;
	Texture myTextures[myTextureCount];

	Shader myFragmentShader;
	Shader myVertexShader;

	unsigned myID;

	CU::Vector4f myAlbedo = {1, 1, 1, 1};

	float mySpecular = 0.5f;
	float mySpecularStrength = 32.f;
};