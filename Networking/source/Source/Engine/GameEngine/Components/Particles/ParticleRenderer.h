#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include <memory>

#include <wrl.h>

#include "Particle.h"

#include "../../ModelSprite/ModelFactory.h"

struct ParticleVertex
{
	ParticleVertex() = default;

	ParticleVertex(const CommonUtilities::Vector4<float>& Position,
		const CommonUtilities::Vector4<float>& Color,
		const CommonUtilities::Vector3<float>& Velocity,
		float Lifetime,
		float Angle,
		const CommonUtilities::Vector2<float>& Size,
		const CommonUtilities::Vector4<float>& ChannelMask)
		:
		Position(Position),
		Color(Color),
		Velocity(Velocity),
		Lifetime(Lifetime),
		Angle(Angle), Size(Size),
		ChannelMask(ChannelMask)
	{
	}

	CommonUtilities::Vector4<float> Position;
	CommonUtilities::Vector4<float> Color;
	CommonUtilities::Vector4<float> ChannelMask;
	CommonUtilities::Vector3<float> Velocity;
	float Lifetime;
	float Angle;
	CommonUtilities::Vector2<float> Size;


	static inline const std::vector<VertexElementDesc> InputLayoutDefenition =
	{
		{ "SV_POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT },
		{ "CHANNELMASK", 0, DXGI_FORMAT_R32G32B32A32_FLOAT },
		{ "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT },
		{ "LIFETIME", 0, DXGI_FORMAT_R32_FLOAT },
		{ "ANGLE", 0, DXGI_FORMAT_R32_FLOAT },
		{ "Size", 0, DXGI_FORMAT_R32G32_FLOAT },
	};
};

struct ID3D11ShaderResourceView;

class ParticleRenderer
{
public:
	~ParticleRenderer() = default;

	ParticleRenderer();

	void Draw();

public:
	std::shared_ptr<Shader> pixelShaderOverride;
	std::shared_ptr<Shader> vertexShaderOverride;
	std::shared_ptr<Shader> geometryShaderOverride;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;

	float maxParticles;

	std::vector<ParticleVertex> myVerticies;
};
