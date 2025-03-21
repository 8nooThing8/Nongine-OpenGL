#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include <memory>

#include <wrl.h>

#include "TrailPoint.h"

#include "../../ModelSprite/ModelFactory.h"

#include "TrailVertex.h"

class Trail;
struct ID3D11ShaderResourceView;

class TrailRenderer
{
public:
	~TrailRenderer() = default;

	TrailRenderer();

	void Draw(const Trail* aTrail);

public:
	std::shared_ptr<Shader> pixelShaderOverride;
	std::shared_ptr<Shader> vertexShaderOverride;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;


	Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> myIndexBuffer;
};
