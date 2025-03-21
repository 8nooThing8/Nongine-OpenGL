#include "../Engine.pch.h"
#include "Shader.h"

#include "RenderHardwareInterface.h"

#include "../GameEngine/Console.h"

void Shader::SetDiffuseTexture(const std::wstring& aPath)
{
	std::filesystem::path path(aPath);

	ID3D11Resource** texture = nullptr;

	HRESULT result = DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetRHI()->myDevice.Get(), aPath.c_str(), texture, myDiffuseTextureShaderResourceViews.GetAddressOf());

	if (FAILED(result))
	{
		PrintC(Console::TypeOfMessage::Error, "Unable to initiate a diffuse at path");
	}

	if (!myDiffuseTextureShaderResourceViews)
		return;

	std::string name = "Diffuse: " + path.filename().string();
	myDiffuseTextureShaderResourceViews->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.length()), name.c_str());
}
void Shader::SetNormalTexture(const std::wstring& aPath)
{
	std::filesystem::path path(aPath);

	ID3D11Resource** texture = nullptr;

	HRESULT result = DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetRHI()->myDevice.Get(), aPath.c_str(), texture, myNormalTextureShaderResourceViews.GetAddressOf());

	if (FAILED(result))
	{
		PrintC(Console::TypeOfMessage::Error, "Unable to initiate a normal at path");
	}

	if (!myNormalTextureShaderResourceViews)
		return;

	std::string name = "Normal: " + path.filename().string();
	myNormalTextureShaderResourceViews->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.length()), name.c_str());
}
void Shader::SetRoughnessTexture(const std::wstring& aPath)
{
	std::filesystem::path path(aPath);

	ID3D11Resource** texture = nullptr;

	HRESULT result = DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetRHI()->myDevice.Get(), aPath.c_str(), texture, myRoughnessTextureShaderResourceViews.GetAddressOf());

	if (FAILED(result))
	{
		PrintC(Console::TypeOfMessage::Error, "Unable to initiate a roughness at path");
	}

	if (!myRoughnessTextureShaderResourceViews)
		return;

	std::string name = "Roughness: " + path.filename().string();
	myRoughnessTextureShaderResourceViews->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.length()), name.c_str());
}
void Shader::SetMetallicTexture(const std::wstring& aPath)
{
	std::filesystem::path path(aPath);

	ID3D11Resource** texture = nullptr;

	HRESULT result = DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetRHI()->myDevice.Get(), aPath.c_str(), texture, myMetallicTextureShaderResourceViews.GetAddressOf());

	if (FAILED(result))
	{
		PrintC(Console::TypeOfMessage::Error, "Unable to initiate a metallic at path");
	}

	if (!myMetallicTextureShaderResourceViews)
		return;

	std::string name = "Metallic: " + path.filename().string();
	myMetallicTextureShaderResourceViews->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.length()), name.c_str());
}

void Shader::SetAOTexture(const std::wstring& aPath)
{
	std::filesystem::path path(aPath);

	ID3D11Resource** texture = nullptr;

	HRESULT result = DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetRHI()->myDevice.Get(), aPath.c_str(), texture, myAOTextureShaderResourceViews.GetAddressOf());

	if (FAILED(result))
	{
		PrintC(Console::TypeOfMessage::Error, "Unable to initiate a diffuse at path");
	}

	if (!myAOTextureShaderResourceViews)
		return;

	std::string name = "AO: " + path.filename().string();
	myAOTextureShaderResourceViews->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.length()), name.c_str());
}

Shader::Shader(const std::wstring& aDiffuseTexture, const std::wstring& aNormalTexture)
{
	myDiffuseTexturePath = std::filesystem::path(aDiffuseTexture).string();
	myNormalTexturePath = std::filesystem::path(aNormalTexture).string();

	ID3D11Resource** texture = nullptr;

	if (!aDiffuseTexture.empty())
	{
		HRESULT result = DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetRHI()->myDevice.Get(), aDiffuseTexture.c_str(), texture, myDiffuseTextureShaderResourceViews.GetAddressOf());

		if (FAILED(result))
		{
			PrintC(Console::TypeOfMessage::Error, "Unable to initiate a shader");
		}

		std::string name = "Diffuse: ";
		name += std::filesystem::path(aDiffuseTexture).filename().string();

		myDiffuseTextureShaderResourceViews->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.length()), name.c_str());
	}

	if (!aNormalTexture.empty())
	{
		HRESULT result = DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetRHI()->myDevice.Get(), aNormalTexture.c_str(), texture, myNormalTextureShaderResourceViews.GetAddressOf());

		if (FAILED(result))
		{
			PrintC(Console::TypeOfMessage::Error, "Unable to initiate a shader");
		}

		std::string name = "Normal: ";
		name += std::filesystem::path(aNormalTexture).filename().string();

		myNormalTextureShaderResourceViews->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.length()), name.c_str());
	}

}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Shader::GetDiffuseShaderResourceView() const
{
	return myDiffuseTextureShaderResourceViews;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Shader::GetNormalShaderResourceView() const
{
	return myNormalTextureShaderResourceViews;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Shader::GetRoughnessShaderResourceView() const
{
	return myRoughnessTextureShaderResourceViews;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Shader::GetMetallicShaderResourceView() const
{
	return myMetallicTextureShaderResourceViews;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Shader::GetAOShaderResourceView() const
{
	return myAOTextureShaderResourceViews;
}

void Shader::SetDiffuseTextureToNull()
{
	myDiffuseTextureShaderResourceViews = nullptr;
}

void Shader::SetNormalTextureToNull()
{
	myNormalTextureShaderResourceViews = nullptr;
}

void Shader::SetRoughnessTextureToNull()
{
	myRoughnessTextureShaderResourceViews = nullptr;
}

void Shader::SetMetallicTextureToNull()
{
	myMetallicTextureShaderResourceViews = nullptr;
}

void Shader::SetAOTextureToNull()
{
	myAOTextureShaderResourceViews = nullptr;
}

Shader::~Shader() = default;

Shader::Shader() = default;