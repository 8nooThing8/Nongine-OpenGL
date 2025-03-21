#pragma once

#include "Component.h"
#include "../../GameEngine/Camera.h"

#include "../../GraphicsEngine/Texture.h"

#include "../../GraphicsEngine/Buffers/FrameBuffer.h"
#include "MeshRenderer.h"

#include "wrl.h"

#include <array>
#include <utility>

#include "GameObject.h"

class ReflectionProbe : public Component
{
public:
	ReflectionProbe();
	~ReflectionProbe()
	{
		delete myCamera;

		MeshRenderer* meshRend = gameObject->GetComponent<MeshRenderer>();

		if (meshRend)
		{
			meshRend->GetMaterial().SetPixelShader(L"Assets/Shaders/PBL_Lit_PS");
		}
		
	}
	
	void AddedAsComponent() override;

	void RotateLightDirection(CommonUtilities::Vector4<float>& lightDir, float angle, char axis);

	void LateUpdateEditor(float aDeltaTime) override;

private:

	std::array<std::pair<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>, CommonUtilities::Matrix4x4<float>>, 6> textures =
	{
		std::pair<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>, CommonUtilities::Matrix4x4<float>>(nullptr, CommonUtilities::Matrix4x4<float>()),
		std::pair<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>, CommonUtilities::Matrix4x4<float>>(nullptr, CommonUtilities::Matrix4x4<float>()),
		std::pair<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>, CommonUtilities::Matrix4x4<float>>(nullptr, CommonUtilities::Matrix4x4<float>()),
		std::pair<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>, CommonUtilities::Matrix4x4<float>>(nullptr, CommonUtilities::Matrix4x4<float>()),
		std::pair<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>, CommonUtilities::Matrix4x4<float>>(nullptr, CommonUtilities::Matrix4x4<float>()),
		std::pair<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>, CommonUtilities::Matrix4x4<float>>(nullptr, CommonUtilities::Matrix4x4<float>())
	};

	std::array<CommonUtilities::Vector4<float>, 6> positioninversion =
	{
		CommonUtilities::Vector4<float>(1, 1, 1, 1),
		CommonUtilities::Vector4<float>(1, 1, 1, 1),
		CommonUtilities::Vector4<float>(1, 1, 1, 1),
		CommonUtilities::Vector4<float>(1, 1, 1, 1),
		CommonUtilities::Vector4<float>(1, 1, 1, 1),
		CommonUtilities::Vector4<float>(1, 1, 1, 1)
	};

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mySRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> myDSV;
	ID3D11Texture2D* cubeTexture = NULL;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mySRV2;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> myDSV2;
	ID3D11Texture2D* cubeTexture2 = NULL;

	int reflectionCount;

	Camera* myCamera;

	D3D11_VIEWPORT myViewport;

	FrameBuffer frameBufferData;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> forwardRenderPS;

	unsigned reflectionQuality;

	int counter = 0;
};
