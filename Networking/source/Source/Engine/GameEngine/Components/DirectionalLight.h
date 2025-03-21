#pragma once

#include "../../GraphicsEngine/Buffers/FrameBuffer.h"
#include "../../GraphicsEngine/Buffers/LightBuffer.h"

#include "Component.h"

class Camera;

struct ID3D11ShaderResourceView;
struct ID3D11DepthStencilView;
struct ID3D11RasterizerState;
struct ID3D11SamplerState;

namespace CU = CommonUtilities;

class DirectionalLight : public Component
{
public:
    DirectionalLight();
	~DirectionalLight() = default;

	CU::Matrix4x4<float> GetMatrix(const bool bNoScale = false) const;

    void EarlyUpdateEditor(float /*aDeltaTime*/) override;

    void RenderImGUI() override;

	void RotateLightDirection(CommonUtilities::Vector4<float>& lightDir, float angle, char axis);

    CommonUtilities::Vector4<float> myLightColor;
    float myIntensity;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mySRV = nullptr;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> myShadowSampler;

private:
	CommonUtilities::Vector4<float> myDirection;

	int lightSize = 1024;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> myDepth = 0;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> myBackFaceCulling = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> myFrontCulling = nullptr;

	Camera* lightCamera;
	D3D11_VIEWPORT myViewports = {};

	FrameBuffer frameBufferData;

	LightBuffer myLightBuffer;
};