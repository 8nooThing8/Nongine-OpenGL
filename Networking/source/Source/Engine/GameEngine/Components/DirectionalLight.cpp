#include "../../Engine.pch.h"

#include "DirectionalLight.h"

#include "../Camera.h"
#include "../ShaderResources.h"

#include "GameObject.h"
#include "../MainSingleton.h"
#include "Transform.h"

#include "../Engine/GameEngine/Components/InstanceComponent.h"

#include <ImGui\imgui.h>

inline void DirectionalLight::RotateLightDirection(CommonUtilities::Vector4<float>& lightDir, const float angle, const char axis)
{
	float s = sin(angle);
	float c = cos(angle);

	CommonUtilities::Vector4<float> rotatedDir;

	switch (axis) {
	case 'x':
	case 'X':
		rotatedDir.x = lightDir.x;
		rotatedDir.y = lightDir.y * c - lightDir.z * s;
		rotatedDir.z = lightDir.y * s + lightDir.z * c;
		break;
	case 'y':
	case 'Y':
		rotatedDir.x = lightDir.x * c + lightDir.z * s;
		rotatedDir.y = lightDir.y;
		rotatedDir.z = -lightDir.x * s + lightDir.z * c;
		break;
	case 'z':
	case 'Z':
		rotatedDir.x = lightDir.x * c - lightDir.y * s;
		rotatedDir.y = lightDir.x * s + lightDir.y * c;
		rotatedDir.z = lightDir.z;
		break;
	}

	lightDir = rotatedDir;
}

inline DirectionalLight::DirectionalLight() : myIntensity(1), myLightColor(CommonUtilities::Vector4<float>(1, 1, 1, 1))
{
	Component();

	lightCamera = new Camera();

	HRESULT result;
	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.Width = lightSize;
	desc.Height = lightSize;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ID3D11Texture2D* texture;
	result = RenderHardwareInterface::myDevice->CreateTexture2D(&desc, nullptr, &texture);
	assert(SUCCEEDED(result));

	ID3D11DepthStencilView* DSV;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};

	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	result = RenderHardwareInterface::myDevice->CreateDepthStencilView(texture, &dsvDesc, &DSV);
	assert(SUCCEEDED(result));

	myDepth = DSV;
	DSV->Release();

	std::string name = "shadow camera";
	myDepth->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.length()), name.c_str());

	ID3D11ShaderResourceView* SRV;
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc{};
	srDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;
	result = RenderHardwareInterface::myDevice->CreateShaderResourceView(texture, &srDesc, &SRV);
	assert(SUCCEEDED(result));
	mySRV = SRV;
	SRV->Release();

	name = "shadow camera";
	mySRV->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.length()), name.c_str());

	// Create the shadow sampler state
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.MipLODBias = 0.0f;
	sampDesc.MaxAnisotropy = 1;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	sampDesc.BorderColor[0] = 0.0f;
	sampDesc.BorderColor[1] = 0.0f;
	sampDesc.BorderColor[2] = 0.0f;
	sampDesc.BorderColor[3] = 0.0f;
	sampDesc.MinLOD = -D3D11_FLOAT32_MAX;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = RenderHardwareInterface::myDevice->CreateSamplerState(&sampDesc, &myShadowSampler);
	if (FAILED(result))
	{
		// Handle error
	}

	name = "Shadow sampler";
	myShadowSampler->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.length()), name.c_str());

	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_FRONT;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.ScissorEnable = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.AntialiasedLineEnable = false;

	result = RenderHardwareInterface::myDevice->CreateRasterizerState(&rasterDesc, myFrontCulling.GetAddressOf());
	if (FAILED(result))
	{
		// fail
		return;
	}

	name = "front face culling";
	myFrontCulling->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.length()), name.c_str());

	rasterDesc.CullMode = D3D11_CULL_BACK;

	result = RenderHardwareInterface::myDevice->CreateRasterizerState(&rasterDesc, myBackFaceCulling.GetAddressOf());
	if (FAILED(result))
	{
		// fail
		return;
	}

	name = "Back face culling";
	myBackFaceCulling->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.length()), name.c_str());

	myViewports = { 0, 0, static_cast<float>(lightSize), static_cast<float>(lightSize), 0, 1 };

	lightCamera->SetOrtographicProjection(CommonUtilities::Vector2<float>(myViewports.Width, myViewports.Height));

	myLightBuffer.pointLights[0].range = 300;

	myLightBuffer.spotLights[0].range = 300;
	myLightBuffer.spotLights[0].cutOffInner = 30.f;
	myLightBuffer.spotLights[0].cutOffOuter = 30.f;
}

CU::Matrix4x4<float> DirectionalLight::GetMatrix(const bool bNoScale) const
{
	bNoScale; 
	CU::Matrix4x4<float> result = CU::Matrix4x4<float>();

	result *= CU::Matrix4x4<float>::CreateRotationAroundX(gameObject->transform->rotation.x);
	result *= CU::Matrix4x4<float>::CreateRotationAroundY(gameObject->transform->rotation.y);
	result *= CU::Matrix4x4<float>::CreateRotationAroundZ(gameObject->transform->rotation.z);

	result.SetPosition(gameObject->transform->position + CommonUtilities::Vector4<float>(0, 0, -99999, 0));

	return result;
}

inline void DirectionalLight::EarlyUpdateEditor(float)
{
	PIXScopedEvent(PIX_COLOR_INDEX(5), L"Directional light");

	auto& context = RenderHardwareInterface::myContext;

	// Initial direction vector, pointing along Z-axis
	CommonUtilities::Vector4<float> lightDir = { 0, -1, 0, 0 };

	RotateLightDirection(lightDir, gameObject->transform->rotation.x - 90, 'x');
	RotateLightDirection(lightDir, gameObject->transform->rotation.y, 'y');
	RotateLightDirection(lightDir, gameObject->transform->rotation.z, 'z');

	myDirection = CommonUtilities::Vector4<float>(lightDir.x, lightDir.y, lightDir.z, 1);

	context->ClearDepthStencilView(myDepth.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	context->OMSetRenderTargets(0, nullptr, myDepth.Get());
	context->RSSetViewports(1, &myViewports);

	CU::Vector4<float> eyeDir = gameObject->transform->GetFinalTransform().GetForward();

	frameBufferData.EyeDir = CU::Vector3<float>(eyeDir.x, eyeDir.y, eyeDir.z);

	CU::Matrix4x4<float> transformationMatrix = GetMatrix();

	frameBufferData.InvView = transformationMatrix.GetInverse();
	frameBufferData.Projection = lightCamera->GetProjectionMatrix();

	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::FrameBuffer, frameBufferData);

	RenderHardwareInterface::myContext->PSSetSamplers(8, 1, myShadowSampler.GetAddressOf());

	RenderHardwareInterface::myContext->RSSetState(myBackFaceCulling.Get());

	myLightBuffer.directionalLights[0].lightViewMatrix = transformationMatrix.GetInverse();
	myLightBuffer.directionalLights[0].lightProjMatrix = lightCamera->GetProjectionMatrix();
	myLightBuffer.directionalLights[0].direction = myDirection;
	myLightBuffer.directionalLights[0].light.color = myLightColor;
	myLightBuffer.directionalLights[0].light.intensity = myIntensity;

	myLightBuffer.pointLights[0].position = gameObject->transform->position;
	myLightBuffer.pointLights[0].light.color = myLightColor;
	myLightBuffer.pointLights[0].light.intensity = myIntensity;

	myLightBuffer.spotLights[0].position = gameObject->transform->position;
	myLightBuffer.spotLights[0].direction = myDirection;
	myLightBuffer.spotLights[0].light.color = myLightColor;
	myLightBuffer.spotLights[0].light.intensity = myIntensity;

	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::LightBuffer, myLightBuffer);



	MainSingleton::Get()->myComponentManager->RenderMeshOverride(gameObject->GetComponent<MeshRenderer>());

	InstanceComponent* instanceComponent = MainSingleton::Get()->instanceComponent;

	if (instanceComponent)
	{
		ObjectBuffer objectBuffer = instanceComponent->gameObject->transform->GetObjBuffer();
		objectBuffer.albedo = CommonUtilities::Vector4<float>(1, 1, 1, 1);

		objectBuffer.isInstanced = true;

		GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objectBuffer);

		GraphicsEngine::Get().RenderMeshes(instanceComponent->instancedMesh, instanceComponent->myInstanceMaterialForward, instanceComponent->myInstancedModels);

	}
	
	RenderHardwareInterface::myContext->RSSetState(myBackFaceCulling.Get());

	MainSingleton::Get()->shaderResources->AddShaderResource(new ShaderResourceItem(16, 1, mySRV.GetAddressOf()));
	MainSingleton::Get()->shaderResources->AddSamplerState(new SamplerStateItem(8, 1, myShadowSampler.GetAddressOf()));
}

inline void DirectionalLight::RenderImGUI()
{
	ImGui::Text("Light color");

	ImGui::SameLine();

	ImGui::ColorPicker4(" ", &myLightColor[0]);

	ImGui::Text("Intensity");

	ImGui::DragFloat("Range", &myLightBuffer.pointLights[0].range, 0.1f);
	myLightBuffer.spotLights[0].range = myLightBuffer.pointLights[0].range;

	ImGui::DragFloat("Inner angle cutoff", &myLightBuffer.spotLights[0].cutOffInner, 0.1f);
	ImGui::DragFloat("Outer angle cutoff", &myLightBuffer.spotLights[0].cutOffOuter, 0.1f);

	ImGui::Button("Directional light");

	ImGui::SameLine();

	ImGui::DragFloat(" ", &myIntensity, 0.1f);

	/*bool whatLightCollapse = ImGui::CollapsingHeader("Type of light");

	if (whatLightCollapse)
	{
		if (ImGui::Button("Directional light"))
		{
			myLightBuffer.lightToUse = 1;
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::Button("Point light"))
		{
			myLightBuffer.lightToUse = 2;
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::Button("spot light"))
		{
			myLightBuffer.lightToUse = 3;
			ImGui::CloseCurrentPopup();
		}
	}*/
}