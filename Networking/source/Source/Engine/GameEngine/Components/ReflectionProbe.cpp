#include "../../Engine.pch.h"

#include "ReflectionProbe.h"
#include "MeshRenderer.h"

#include "../Console.h"
#include "../../GraphicsEngine/ShaderCompiler.h"

#include "../../GraphicsEngine/TextureHelper.hpp"

#include "Transform.h"

#include "../MainSingleton.h"

#include "MeshRenderer.h"

#include "../ShaderResources.h"

void ReflectionProbe::AddedAsComponent()
{
	MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();

	if (!meshRenderer)
	{
		meshRenderer = gameObject->AddComponent<MeshRenderer>();
	}

	meshRenderer->GetMaterial().SetPixelShader(L"Assets/Shaders/ReflectionProbe_PS");

	myCamera = new Camera({ 1.f, 1.f }, (gameObject->transform->scale.x + gameObject->transform->scale.y + gameObject->transform->scale.z) / 3);
}

ReflectionProbe::ReflectionProbe() : reflectionQuality(1024), reflectionCount(1)
{
	Component();

	textures[0].second.SetForward(-1, 0, 0);
	textures[0].second.SetUp(0, 1, 0);
	textures[0].second.SetRight(0, 0, 1);

	textures[1].second.SetForward(1, 0, 0);
	textures[1].second.SetUp(0, 1, 0);
	textures[1].second.SetRight(0, 0, -1);

	textures[2].second.SetForward(0, -1, 0);
	textures[2].second.SetUp(0, 0, 1);
	textures[2].second.SetRight(1, 0, 0);

	textures[3].second.SetForward(0, 1, 0);
	textures[3].second.SetUp(0, 0, -1);
	textures[3].second.SetRight(1, 0, 0);

	textures[4].second.SetForward(0, 0, 1);
	textures[4].second.SetUp(0, 1, 0);
	textures[4].second.SetRight(1, 0, 0);

	textures[5].second.SetForward(0, 0, -1);
	textures[5].second.SetUp(0, 1, 0);
	textures[5].second.SetRight(-1, 0, 0);

	positioninversion[0] = CommonUtilities::Vector4<float>(-1, 1, -1, 1);
	positioninversion[1] = CommonUtilities::Vector4<float>(-1, 1, -1, 1);
	positioninversion[2] = CommonUtilities::Vector4<float>(1, -1, -1, 1);
	positioninversion[3] = CommonUtilities::Vector4<float>(1, -1, -1, 1);



	HRESULT result;
	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.Width = reflectionQuality;
	desc.Height = reflectionQuality;
	desc.MipLevels = 0;
	desc.ArraySize = 6;
	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ID3D11Texture2D* textureDSV;
	result = RenderHardwareInterface::myDevice->CreateTexture2D(&desc, nullptr, &textureDSV);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};

	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	result = RenderHardwareInterface::myDevice->CreateDepthStencilView(textureDSV, &dsvDesc, myDSV.GetAddressOf());

	D3D11_TEXTURE2D_DESC texDesc = {};

	texDesc.Width = reflectionQuality;
	texDesc.Height = reflectionQuality;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 6;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS;

	cubeTexture = { };

	HRESULT hr = GraphicsEngine::GetRHI()->GetDevice()->CreateTexture2D(&texDesc, nullptr, &cubeTexture);
	hr;

	D3D11_RENDER_TARGET_VIEW_DESC description = {};

	description.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	description.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	description.Texture2DArray.ArraySize = 1;
	description.Texture2DArray.MipSlice = 0;

	for (int i = 0; i < textures.size(); i++)
	{
		description.Texture2DArray.FirstArraySlice = i;

		result = RenderHardwareInterface::myDevice->CreateRenderTargetView(cubeTexture, &description, textures[i].first.GetAddressOf());
	}

	std::string name = "Right RTV";
	textures[0].first->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.length()), name.c_str());

	name = "Left RTV";
	textures[1].first->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.length()), name.c_str());

	name = "Up RTV";
	textures[2].first->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.length()), name.c_str());

	name = "Down RTV";
	textures[3].first->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.length()), name.c_str());

	name = "Forward RTV";
	textures[4].first->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.length()), name.c_str());

	name = "back RTV";
	textures[5].first->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.length()), name.c_str());

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc = {};

	viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	viewDesc.TextureCube.MostDetailedMip = 0;
	viewDesc.TextureCube.MipLevels = 10;

	result = RenderHardwareInterface::myDevice->CreateShaderResourceView(cubeTexture, &viewDesc, mySRV.GetAddressOf());

	name = "Reflection cube map";
	mySRV->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.length()), name.c_str());

	//cubeTexture2 = { };

	//hr = GraphicsEngine::GetRHI()->GetDevice()->CreateTexture2D(&texDesc, nullptr, &cubeTexture2);

	//result = RenderHardwareInterface::myDevice->CreateShaderResourceView(cubeTexture2, &viewDesc, mySRV2.GetAddressOf());


	myViewport = { 0, 0, static_cast<float>(reflectionQuality), static_cast<float>(reflectionQuality), 0, 1 };

	forwardRenderPS = Shaders::CompilePixelShader(L"Assets/Shaders/PBL_Lit_PS.hlsl");
}

inline void ReflectionProbe::RotateLightDirection(CommonUtilities::Vector4<float>& lightDir, const float angle, const char axis)
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

void ReflectionProbe::LateUpdateEditor(float aDeltaTime)
{
	auto& context = RenderHardwareInterface::myContext;

	std::array<float, 4> clearColor = { 1, 0, 0, 1 };

	context->RSSetViewports(1, &myViewport);

	MainSingleton::Get()->mySkyBox->transform->position = gameObject->transform->position;
	MainSingleton::Get()->mySkyBox->transform->Update(0);

	for (int i = 0; i < textures.size(); i++)
	{
		context->ClearRenderTargetView(textures[i].first.Get(), clearColor.data());
		context->ClearDepthStencilView(myDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		context->OMSetRenderTargets(1, textures[i].first.GetAddressOf(), myDSV.Get());

		if (!i)
			MainSingleton::Get()->shaderResources->SetShaderObjects();

		//GraphicsEngine::GetRHI()->GetContext()->PSSetShaderResources(32, 1, MainSingleton::Get()->skyBoxTexture.GetAddressOf());
		//MainSingleton::Get()->mySkyBoxMeshRend->Draw();

		CommonUtilities::Matrix4x4<float> matrix;

		matrix = textures[i].second;

		matrix.SetPosition(gameObject->transform->position * textures[i].second * positioninversion[i]);

		frameBufferData.InvView = matrix.GetInverse();

		frameBufferData.Projection = myCamera->GetProjectionMatrix();
		frameBufferData.CameraPos = gameObject->transform->position;

		GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::FrameBuffer, frameBufferData);

		MainSingleton::Get()->myComponentManager->RenderMeshOverride(gameObject->GetComponent<MeshRenderer>(), forwardRenderPS);

	}

	if (counter++ >= 20)
	{
		counter = 0;

	}

	GraphicsEngine::GetRHI()->GetContext()->GenerateMips(mySRV.Get());

	//TextureHelper::CreateTexture3D(L"Assets/Shaders/ReWrite3D_PS.hlsl", 60, mySRV.Get(), mySRV2);



	//GraphicsEngine::GetRHI()->GetContext()->GenerateMips(mySRV2.Get());


	gameObject->GetComponent<MeshRenderer>()->shaderItems->AddShaderResource(new ShaderResourceItem(64, 1, mySRV.GetAddressOf()));

	if (aDeltaTime < static_cast<float>(reflectionCount - 1))
	{
		MainSingleton::Get()->myComponentManager->ReflectionOverride(this, ++aDeltaTime);
	}
}
