#pragma once

#include "ShaderCompiler.h"

#ifdef OPENGL

#include <string>

class Shader
{
public:
	Shader() = default;
	~Shader() = default;
	Shader(const char* aPath, ShaderCompiler::ShaderType aType);

	unsigned GetID();
	void Use(unsigned aProgramID);

private:
	std::string myPath;
	unsigned myID;

	bool isNull = false;
};

#elif D3D11

#include <wrl.h>

struct ID3D11DeviceChild;
struct ID3D11ShaderResourceView;

enum class ShaderType : unsigned
{
	Unknown,
	VertexShader,
	GeometryShader,
	PixelShader
};

class Shader
{
	friend class GraphicsEngine;
	friend class RenderHardwareInterface;

public:
	Shader(const std::wstring& aDiffuseTexture, const std::wstring& aNormalTexture = L"");
	Shader();
	~Shader();

	FORCEINLINE ShaderType GetShaderType() const { return myType; }
	FORCEINLINE std::wstring GetShaderName() const { return myName; }
	FORCEINLINE std::wstring GetShaderPath() const { return myPath; }

	FORCEINLINE std::string GetDiffusePath() const { return myDiffuseTexturePath; }
	FORCEINLINE std::string GetNormalPath() const { return myNormalTexturePath; }
	FORCEINLINE std::string GetRoughnessPath() const { return myRoughnessTexturePath; }
	FORCEINLINE std::string GetMetallicPath() const { return myMetallicTexturePath; }
	FORCEINLINE std::string GetAOPath() const { return myAOTexturePath; }

	FORCEINLINE std::string GetDiffuseName() const { return myDiffuseName; }
	FORCEINLINE std::string GetNormalName() const { return myNormalName; }

	void SetDiffuseTexture(const std::wstring& aPath);
	void SetNormalTexture(const std::wstring& aPath);
	void SetRoughnessTexture(const std::wstring& aPath);
	void SetMetallicTexture(const std::wstring& aPath);
	void SetAOTexture(const std::wstring& aPath);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetDiffuseShaderResourceView() const;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetNormalShaderResourceView() const;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetRoughnessShaderResourceView() const;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetMetallicShaderResourceView() const;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetAOShaderResourceView() const;

	void SetDiffuseTextureToNull();
	void SetNormalTextureToNull();
	void SetRoughnessTextureToNull();
	void SetMetallicTextureToNull();
	void SetAOTextureToNull();


	void SetShader(Microsoft::WRL::ComPtr<ID3D11DeviceChild> aShader) { myshader = aShader; }

	void SetShaderNamePath(const std::wstring& aName, const std::wstring& aPath) { myName = aName; myPath = aPath; }

	void SetDiffuseName(const std::string& aDiffuseName) { myDiffuseName = aDiffuseName; }
	void SetNormalName(const std::string& aNormalName) { myNormalName = aNormalName; }
	void SetRoughnessName(const std::string& aRoughnessName) { myRoughnessName = aRoughnessName; }
	void SetMetalliclName(const std::string& aMetallicName) { myMetallicName = aMetallicName; }
	void SetAOName(const std::string& aAOName) { myAOName = aAOName; }

	void SetDiffusePath  (const std::string& aDiffusePath)   { myDiffuseTexturePath = aDiffusePath;   }
	void SetNormalPath   (const std::string& aNormalPath)    { myNormalTexturePath = aNormalPath;    }
	void SetRoughnessPath(const std::string& aRoughnessPath) { myRoughnessTexturePath = aRoughnessPath; }
	void SetMetalliclPath(const std::string& aMetallicPath)  { myMetallicTexturePath = aMetallicPath;  }
	void SetAOPath       (const std::string& aAOPath)        { myAOTexturePath = aAOPath;        }
private:

	std::wstring myName;
	std::wstring myPath;

	ShaderType myType = ShaderType::Unknown;
	Microsoft::WRL::ComPtr<ID3D11DeviceChild> myshader;

	std::string myDiffuseTexturePath;
	std::string myNormalTexturePath;
	std::string myRoughnessTexturePath;
	std::string myMetallicTexturePath;
	std::string myAOTexturePath;

	std::string myDiffuseName;
	std::string myNormalName;
	std::string myRoughnessName;
	std::string myMetallicName;
	std::string myAOName;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myDiffuseTextureShaderResourceViews;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myNormalTextureShaderResourceViews;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myRoughnessTextureShaderResourceViews;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myMetallicTextureShaderResourceViews;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myAOTextureShaderResourceViews;

};

#endif