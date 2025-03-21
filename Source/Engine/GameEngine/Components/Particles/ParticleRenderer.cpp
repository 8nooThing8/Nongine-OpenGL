#include "../../../Engine.pch.h"

#include "ParticleRenderer.h"

#include "../../../GraphicsEngine/Buffers/ObjectBuffer.h"
#include "../../../GraphicsEngine/GraphicsEngine.h"
#include "../../../GraphicsEngine/RenderHardwareInterface.h"

#include <d3d11.h>

#include "../../Camera.h"
#include "../Engine/GraphicsEngine/Shader.h"
#include "../Engine/GraphicsEngine/ShaderCompiler.h"

#include "../Engine/GameEngine/Console.h"

#include "../engine/GameEngine/GameCamera.h"

struct ParticleSorting
{
	inline bool operator() (ParticleVertex pa1, ParticleVertex pa2)
	{
		float distanceToCamera1;
		float distanceToCamera2;

		distanceToCamera1 = (pa1.Position - GameCamera::main->position).Length();
		distanceToCamera2 = (pa2.Position - GameCamera::main->position).Length();

		return distanceToCamera1 > distanceToCamera2;
	}
};

ParticleRenderer::ParticleRenderer()
{
	pixelShaderOverride = std::make_shared<Shader>(L"", L"");
	vertexShaderOverride = std::make_shared<Shader>(L"", L"");
	geometryShaderOverride = std::make_shared<Shader>(L"", L"");

	pixelShaderOverride->SetShaderNamePath(L"ParticlePixelShader", L"Assets/Shaders/Particle_Smoke_PS.hlsl");
	vertexShaderOverride->SetShaderNamePath(L"ParticleVertexShader", L"Assets/Shaders/Particle_VS.hlsl");
	geometryShaderOverride->SetShaderNamePath(L"ParticleGeometryShader", L"Assets/Shaders/Particle_GS.hlsl");

	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader = Shaders::CompilePixelShader(pixelShaderOverride->GetShaderPath());
	pixelShaderOverride->SetShader(pixelShader);

	Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometryShader = Shaders::CompileGeometryShader(geometryShaderOverride->GetShaderPath());
	geometryShaderOverride->SetShader(geometryShader);

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

	sucess = DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetRHI()->GetDevice().Get(), L"Assets/Textures/SpriteCloud.dds", &textureResource, texture.GetAddressOf());

	if (FAILED(sucess))
	{
		PrintError("Error in loading particle texture");
	}

	const void* bytecode = blob->GetBufferPointer();
	size_t bytecodeSize = blob->GetBufferSize();

	bool success = GraphicsEngine::GetRHI()->CreateInputLayout(inputLayout, ParticleVertex::InputLayoutDefenition, bytecode, bytecodeSize);
	success;
	if (FAILED(sucess))
	{
		PrintError("Error making inputlayout");
	}

	myVerticies.reserve(65536);

	GraphicsEngine::GetRHI()->CreateVertexBuffer<ParticleVertex>("Particle vertex buffer", myVerticies, myVertexBuffer, true);
}

CommonUtilities::Matrix4x4<float> GetFinalTransform(const Particle& aParticle)
{
	CommonUtilities::Matrix4x4<float> returnMX = CommonUtilities::Matrix4x4<float>();

	returnMX(1, 1) = aParticle.scale.x;
	returnMX(2, 2) = aParticle.scale.y;
	returnMX(3, 3) = aParticle.scale.z;

	CU::Vector3<float> rotation = GameCamera::main->rotation;

	CommonUtilities::Matrix4x4<float> positionRotation;

	positionRotation = CommonUtilities::Matrix4x4<float>::CreateRotationAsQuerternion(CommonUtilities::Vector3<float>(-rotation.x * CU::deg2rad, rotation.y * CU::deg2rad, 0).ToQuart());

	positionRotation.SetPosition(aParticle.position);

	returnMX *= positionRotation;

	return returnMX;
}

void ParticleRenderer::Draw()
{
	std::sort(myVerticies.begin(), myVerticies.end(), [](ParticleVertex pa1, ParticleVertex pa2) {

		const float distanceToCamera1 = (pa1.Position - GameCamera::main->position).Length();
		const float distanceToCamera2 = (pa2.Position - GameCamera::main->position).Length();

		return distanceToCamera1 > distanceToCamera2;
		});

	GraphicsEngine::GetRHI()->UpdateVertexBuffer(myVerticies, myVertexBuffer);

	GraphicsEngine::GetRHI().get()->SetInputLayout(inputLayout);

	ObjectBuffer objectBuffer;

	//objectBuffer.world = CommonUtilities::Matrix4x4<float>();
	objectBuffer.rotation = objectBuffer.world;

	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objectBuffer);

	auto& rhi = GraphicsEngine::GetRHI();

	rhi->SetVertexBuffer(myVertexBuffer, sizeof(ParticleVertex), 0);

	rhi->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	rhi->SetShaders(vertexShaderOverride.get(), pixelShaderOverride.get());
	rhi->SetGeometryShader(*geometryShaderOverride);

	rhi->DrawVertexTexture(static_cast<unsigned>(myVerticies.size()), texture);

	rhi->ResetGeometryShader();

	myVerticies.clear();
}