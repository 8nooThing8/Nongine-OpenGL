#include "../../../Engine.pch.h"

#include "TrailRenderer.h"

#include "../../../GraphicsEngine/Buffers/ObjectBuffer.h"
#include "../../../GraphicsEngine/GraphicsEngine.h"
#include "../../../GraphicsEngine/RenderHardwareInterface.h"

#include <d3d11.h>


#include "../Engine/GraphicsEngine/Shader.h"
#include "../Engine/GraphicsEngine/ShaderCompiler.h"

#include "../Engine/GameEngine/Console.h"

#include "Trail.h"



TrailRenderer::TrailRenderer()
{
	pixelShaderOverride = std::make_shared<Shader>(L"", L"");
	vertexShaderOverride = std::make_shared<Shader>(L"", L"");

	pixelShaderOverride->SetShaderNamePath(L"ParticleShaderPixelShader", L"Assets/Shaders/Trail_PS.hlsl");
	vertexShaderOverride->SetShaderNamePath(L"ParticleShaderVertexShader", L"Assets/Shaders/Trail_VS.hlsl");

	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader = Shaders::CompilePixelShader(pixelShaderOverride->GetShaderPath());

	pixelShaderOverride->SetShader(pixelShader);

	Microsoft::WRL::ComPtr<ID3D10Blob> blob;

	Shaders::CompileVertexShader(vertexShaderOverride->GetShaderPath(), blob);

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;

	HRESULT sucess = GraphicsEngine::GetRHI()->GetDevice()->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, vertexShader.GetAddressOf());

	if (FAILED(sucess))
	{
		PrintError("Error in loading particle texture");
	}

	vertexShaderOverride->SetShader(vertexShader);

	ID3D11Resource* textureResource = nullptr;

	sucess = DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetRHI()->GetDevice().Get(), L"Assets/Textures/negz.dds", &textureResource, texture.GetAddressOf());

	if (FAILED(sucess))
	{
		PrintError("Error in loading particle texture");
	}

	const void* bytecode = blob->GetBufferPointer();
	size_t bytecodeSize = blob->GetBufferSize();

	bool success = GraphicsEngine::GetRHI()->CreateInputLayout(inputLayout, TrailVertex::InputLayoutDefenition, bytecode, bytecodeSize);
	success;
	if (FAILED(sucess))
	{
		PrintError("Error making inputlayout");
	}

	std::vector<unsigned> inspector;
	std::vector<TrailVertex> myVerticies;

	myVerticies.reserve(16384 * 4);
	inspector.reserve(16384 * 4);

	myVerticies =
	{
		TrailVertex(CommonUtilities::Vector4<float>(-1, -1, 0, 1), CommonUtilities::Vector2<float>(0, 1)), // Bottom-left
		TrailVertex(CommonUtilities::Vector4<float>(1, -1, 0, 1), CommonUtilities::Vector2<float>(1, 1)),  // Bottom-right
		TrailVertex(CommonUtilities::Vector4<float>(1, 1, 0, 1), CommonUtilities::Vector2<float>(1, 0)),   // Top-right
		TrailVertex(CommonUtilities::Vector4<float>(-1, 1, 0, 1), CommonUtilities::Vector2<float>(0, 0))   // Top-left
	};

	inspector = { 3, 2, 0, 2, 1, 0 };

	GraphicsEngine::GetRHI()->CreateVertexBuffer<TrailVertex>("Particle vertex buffer", myVerticies, myVertexBuffer, true);
	GraphicsEngine::GetRHI()->CreateIndexBuffer("Particle index buffer", inspector, myIndexBuffer, true);
}



void TrailRenderer::Draw(const Trail* aTrail)
{
	GraphicsEngine::GetRHI().get()->SetInputLayout(inputLayout);

	CommonUtilities::Matrix4x4<float> finalTransform;

	ObjectBuffer objectBuffer;

	objectBuffer.albedo = CU::Vector4<float>(1, 1, 1, 1);
	objectBuffer.world = finalTransform;

	

	objectBuffer.scale = CU::Vector4<float>(0, 0, 0, 0);

	finalTransform.SetScale(objectBuffer.scale);

	objectBuffer.rotation = finalTransform;

	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objectBuffer);

	auto& rhi = GraphicsEngine::GetRHI();

	rhi->SetVertexBuffer(myVertexBuffer, sizeof(TrailVertex), 0);
	rhi->SetIndexBuffer(myIndexBuffer);

	rhi->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	rhi->SetShaders(*vertexShaderOverride, *pixelShaderOverride);

	rhi->DrawIndexedTexture(0, static_cast<unsigned>(aTrail->myTrailIndecies.size()), 0, texture);
}