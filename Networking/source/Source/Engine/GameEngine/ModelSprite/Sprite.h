#pragma once

#include <vector>
#include <wrl.h>

#include "filesystem"
#include "d3d11.h"

#include "Vector/Vector.hpp"

#include "../../GameEngine/Console.h"

#include "../../GraphicsEngine/DDSTextureLoader11.h"
#include "../../GraphicsEngine//RenderHardwareInterface.h"

#include "../../GraphicsEngine/ShaderCompiler.h"

struct Vert
{
	Vert() = default;

	Vert(const CommonUtilities::Vector2<float> aPos, const CommonUtilities::Vector2<float> aUV) : Position(aPos), UV(aUV)
	{

	}

	CommonUtilities::Vector2<float> Position = { 0, 0 };
	CommonUtilities::Vector2<float> UV = { 0, 0 };

	bool operator==(const Vert& other) const
	{
		// A vertex is just a POD object so we can do this.
		return memcmp(this, &other, sizeof(Vert)) == 0;
	}
};

struct Sprite
{
public:
	static inline const std::vector<VertexElementDesc> InputLayoutDefenition =
	{
		{ "SV_POSITION", 0, DXGI_FORMAT_R32G32_FLOAT },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT },
	};

	friend class SpriteRenderer;

	std::filesystem::path mytexturePath;

	Sprite(std::filesystem::path aPath = std::filesystem::path("")) : mytexturePath(aPath)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> blob;

		Shaders::CompileVertexShader(L"Assets/Shaders/Sprite_VS.hlsl", blob);

		const void* bytecode = blob->GetBufferPointer();
		size_t bytecodeSize = blob->GetBufferSize();

		bool success = GraphicsEngine::GetRHI()->CreateInputLayout(myInputLayout, InputLayoutDefenition, bytecode, bytecodeSize);

		if (!success)
		{
			PrintC(Console::Error, "Failed to create input layout for sprite");
		}

		ID3D11Resource** texture = nullptr;

		HRESULT result = S_OK;

		if (!aPath.empty())
		{
			result = DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetRHI()->myDevice.Get(), aPath.wstring().c_str(), texture, aTexture.GetAddressOf());
		}

		if (FAILED(result))
		{
			PrintC(Console::TypeOfMessage::Error, "Unable to initiate a Texture at path");
		}

		myVerticies =
		{
			Vert(CommonUtilities::Vector2<float>(-0.5f, -0.5f), CommonUtilities::Vector2<float>(0, 1)),
			Vert(CommonUtilities::Vector2<float>(-0.5f,  0.5f), CommonUtilities::Vector2<float>(0, 0)),
			Vert(CommonUtilities::Vector2<float>(0.5f,  0.5f), CommonUtilities::Vector2<float>(1, 0)),
			Vert(CommonUtilities::Vector2<float>(0.5f, -0.5f), CommonUtilities::Vector2<float>(1, 1)),
		};

		myIndices = { 0, 1, 2, 0, 2, 3 };

		GraphicsEngine::GetRHI()->CreateVertexBuffer<Vert>("VertexBuffer", myVerticies, myVertexBuffer);
		GraphicsEngine::GetRHI()->CreateIndexBuffer("IndexBuffer", myIndices, myIndexBuffer);
	}

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> aTexture;

	Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> myIndexBuffer;
private:

	Microsoft::WRL::ComPtr<ID3D11InputLayout> myInputLayout = nullptr;

	std::vector<unsigned> myIndices;

	std::vector<Vert> myVerticies;

};