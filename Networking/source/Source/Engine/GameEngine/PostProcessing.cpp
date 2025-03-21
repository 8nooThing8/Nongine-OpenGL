#include "../Engine.pch.h"

#include "PostProcessing.h"

#include "../GraphicsEngine/GraphicsEngine.h"

#include "Components/SpriteRenderer.h"

#include "../GraphicsEngine/Buffers/DebugBuffer.h"
#include "../GraphicsEngine/Buffers/PostProcessBuffer.h"

#include "../GameEngine/ModelSprite/Sprite.h"
#include "../GameEngine/Components/GameObject.h"

#include "../GraphicsEngine/Buffers/SSAO.h"

#include <random>

#include <InputHandler.h>

#include "MainSingleton.h"
#include "ShaderResources.h"
#include "Components/Transform.h"

int findFirstSetBitPosition(const int value)
{
	if (value == 0) return 0;

	unsigned long index;
	if (_BitScanForward(&index, value))
	{
		return static_cast<int>(index);
	}

	return 0;
}

PostProcessResource::PostProcessResource(PPType aPPType, std::shared_ptr <D3D11_TEXTURE2D_DESC> aTD) : textureDesc(aTD), type(aPPType)
{

}

ID3D11ShaderResourceView* PostProcessing::Render(ID3D11ShaderResourceView* inSRV)
{
	PIXScopedEvent(PIX_COLOR_INDEX(2), L"Post process render");

	ID3D11ShaderResourceView* nullViews[] = { nullptr };

	for (int i = 0; i < textures.size(); i++)
	{
		GraphicsEngine::GetRHI()->GetContext()->PSSetShaderResources(80 + textures[i]->type, 1, nullViews);
		GraphicsEngine::GetRHI()->ClearRenderTargetView(textures[i]->rtv);
	}

	mySprite->transform->scale = CommonUtilities::Vector4<float>(1920, 1080, 1, 1);
	mySprite->transform->Update(0);

	if (InputHandler::GetInstance()->GetKeyDown(keycode::F7))
	{
		tonemap++;

		if (tonemap > 1)
		{
			tonemap = 0;
		}
	}


	DebugBuffer debug;

	debug.toneMapUsage = tonemap;

	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::DebugBuffer, debug);

	PostProcessBuffer ppBuffer;

	ppBuffer.textureResolution = GraphicsEngine::Get().myGameResolution;

	PIXBeginEvent(PIX_COLOR_INDEX(2), L"Bloom mask");

	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::PostProcess, ppBuffer);

	GraphicsEngine::GetRHI()->SetRenderTargetView(textures[0]->rtv);
	GraphicsEngine::GetRHI()->GetContext()->PSSetSamplers(5, 1, myPostProcessSampler.GetAddressOf());
	GraphicsEngine::GetRHI()->GetContext()->PSSetShaderResources(80, 1, &inSRV);

	textures[0]->spriteRenderer->LateUpdateEditor(0);
	MainSingleton::Get()->shaderResources->AddShaderResource(new ShaderResourceItem(81, 1, textures[0]->srv.GetAddressOf()));

	PIXEndEvent();

	PIXBeginEvent(PIX_COLOR_INDEX(2), L"Image downscaling");

	ppBuffer.downsampledTextureResolution = GraphicsEngine::Get().myGameResolution * 0.5f;
	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::PostProcess, ppBuffer);

	GraphicsEngine::GetRHI()->SetRenderTargetView(textures[1]->rtv);
	GraphicsEngine::GetRHI()->GetContext()->PSSetShaderResources(81, 1, textures[0]->srv.GetAddressOf());
	textures[1]->spriteRenderer->LateUpdateEditor(0);


	ppBuffer.downsampledTextureResolution = GraphicsEngine::Get().myGameResolution * 0.25f;
	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::PostProcess, ppBuffer);

	GraphicsEngine::GetRHI()->SetRenderTargetView(textures[2]->rtv);
	GraphicsEngine::GetRHI()->GetContext()->PSSetShaderResources(81, 1, textures[1]->srv.GetAddressOf());
	textures[2]->spriteRenderer->LateUpdateEditor(0);


	ppBuffer.downsampledTextureResolution = GraphicsEngine::Get().myGameResolution * 0.125f;
	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::PostProcess, ppBuffer);

	GraphicsEngine::GetRHI()->SetRenderTargetView(textures[3]->rtv);
	GraphicsEngine::GetRHI()->GetContext()->PSSetShaderResources(81, 1, textures[2]->srv.GetAddressOf());
	textures[3]->spriteRenderer->LateUpdateEditor(0);

	PIXEndEvent();

	PIXBeginEvent(PIX_COLOR_INDEX(2), L"Gausian blur");

	GraphicsEngine::GetRHI()->SetRenderTargetView(textures[4]->rtv);
	GraphicsEngine::GetRHI()->GetContext()->PSSetShaderResources(81, 1, textures[3]->srv.GetAddressOf());
	textures[4]->spriteRenderer->LateUpdateEditor(0);

	GraphicsEngine::GetRHI()->SetRenderTargetView(textures[5]->rtv);
	GraphicsEngine::GetRHI()->GetContext()->PSSetShaderResources(81, 1, textures[4]->srv.GetAddressOf());
	textures[5]->spriteRenderer->LateUpdateEditor(0);

	MainSingleton::Get()->shaderResources->AddShaderResource(new ShaderResourceItem(82, 1, textures[5]->srv.GetAddressOf()));

	PIXEndEvent();

	PIXBeginEvent(PIX_COLOR_INDEX(2), L"SSAO bluring");

	GraphicsEngine::GetRHI()->SetRenderTargetView(textures[textures.size() - 1]->rtv);
	GraphicsEngine::GetRHI()->GetContext()->PSSetShaderResources(80, 1, &inSRV);

	MainSingleton::Get()->shaderResources->SetShaderObjects();
	MainSingleton::Get()->shaderResources->CleanUp();

	textures[textures.size() - 1]->spriteRenderer->LateUpdateEditor(0);

	PIXEndEvent();

	return textures[textures.size() - 1]->srv.Get();
	//return inSRV;
}

int PostProcessing::GetPPEffectsCount()
{
	int count = 0;

	for (int i = 0; i < PPEffectCount + 1; i++)
	{
		if (postProcessUsing & (1 << i))
		{
			count++;
		}
	}

	return count;
}

int PostProcessing::GetNextPPEffect(const int aNext)
{
	for (int i = aNext; i < PPEffectCount + 1; i++)
	{
		if (postProcessUsing & (1 << i))
		{
			return postProcessUsing & (1 << i);
		}
	}

	throw("could not find a valid next PP effect");
}

std::wstring EnumToShader(const PPType aInput)
{
	switch (aInput)
	{
	case SSAOPP:
		return L"Assets/Shaders/SSAO.hlsl";
	case Bloom:
		return L"Assets/Shaders/Bloom_PS.hlsl";
	case DownSample:
		return L"Assets/Shaders/DownSample_PS.hlsl";
	case UpSample:
		return L"Assets/Shaders/UpSample_PS.hlsl";
	case GaussianBlurHor:
		return L"Assets/Shaders/GaussianBlurHorizontal_PS.hlsl";
	case GaussianBlurVer:
		return L"Assets/Shaders/GaussianBlurVertical_PS.hlsl";
	case ToneMap:
		return L"Assets/Shaders/ToneMapping_PS.hlsl";
	}

	return L"Error";
}

void PostProcessing::Init(const HWND aHWND)
{
	std::uniform_real_distribution<float> randomValues(0, 1);
	std::default_random_engine rng;

	SSAO PostProcessData;

	for (unsigned i = 0; i < kernelSize; ++i)
	{
		CommonUtilities::Vector4<float> v = {
		randomValues(rng) * 2.0f - 1.0f,
		randomValues(rng) * 2.0f - 1.0f,
		randomValues(rng) , 0 };
		v = v.GetSafeNormal();
		float s = static_cast<float>(i) / static_cast<float>(kernelSize);
		s = std::lerp(0.1f, 1.0f, s * s);
		v *= s;
		PostProcessData.Kernel[i] = v;
	}

	ConstantBuffer ssao;

	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::SSAO, PostProcessData);

	ID3D11Resource** tex = nullptr;

	DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetRHI()->myDevice.Get(), L"Assets/Textures/LDR_LLL1_0.dds", tex, myBlueNoise.GetAddressOf());

	GraphicsEngine::GetRHI()->GetContext()->PSSetShaderResources(19, 1, myBlueNoise.GetAddressOf());

	HRESULT hr;

	RECT clientRect = {};
	GetClientRect(aHWND, &clientRect);

	const float clientWidth = static_cast<float>(clientRect.right - clientRect.left);
	const float clientHeight = static_cast<float>(clientRect.bottom - clientRect.top);

	// Create the texture description
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = static_cast<unsigned>(clientWidth);
	textureDesc.Height = static_cast<unsigned>(clientHeight);
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	Microsoft::WRL::ComPtr<ID3D11Device>& device = GraphicsEngine::GetRHI()->GetDevice();

	// Create the shadow sampler state
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&samplerDesc, myPostProcessSampler.GetAddressOf());

	myPostProcessSampler->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Post process sampler").length()), "Post process sampler");

	mySprite = std::make_shared<GameObject>("aSprite");

	textures.push_back(new PostProcessResource(PPType::Bloom, std::make_shared<D3D11_TEXTURE2D_DESC>(textureDesc)));

	textureDesc.Width = static_cast<unsigned>(clientWidth / 2);
	textureDesc.Height = static_cast<unsigned>(clientHeight / 2);

	textures.push_back(new PostProcessResource(PPType::DownSample, std::make_shared<D3D11_TEXTURE2D_DESC>(textureDesc)));

	textureDesc.Width = static_cast<unsigned>(clientWidth / 4);
	textureDesc.Height = static_cast<unsigned>(clientHeight / 4);

	textures.push_back(new PostProcessResource(PPType::DownSample, std::make_shared<D3D11_TEXTURE2D_DESC>(textureDesc)));

	textureDesc.Width = static_cast<unsigned>(clientWidth / 8);
	textureDesc.Height = static_cast<unsigned>(clientHeight / 8);

	textures.push_back(new PostProcessResource(PPType::DownSample, std::make_shared<D3D11_TEXTURE2D_DESC>(textureDesc)));

	textureDesc.Width = static_cast<unsigned>(clientWidth);
	textureDesc.Height = static_cast<unsigned>(clientHeight);

	textures.push_back(new PostProcessResource(PPType::GaussianBlurHor, std::make_shared<D3D11_TEXTURE2D_DESC>(textureDesc)));
	textures.push_back(new PostProcessResource(PPType::GaussianBlurVer, std::make_shared<D3D11_TEXTURE2D_DESC>(textureDesc)));

	textures.push_back(new PostProcessResource(PPType::SSAOPP, std::make_shared<D3D11_TEXTURE2D_DESC>(textureDesc)));

	//textureDesc.Width = static_cast<unsigned>(clientWidth / 4);
	//textureDesc.Height = static_cast<unsigned>(clientHeight / 4);

	//textures.push_back(new PostProcessResource(PPType::UpSample, textureDesc));

	//textureDesc.Width = static_cast<unsigned>(clientWidth / 2);
	//textureDesc.Height = static_cast<unsigned>(clientHeight / 2);

	//textures.push_back(new PostProcessResource(PPType::UpSample, textureDesc));
	//
	//textureDesc.Width = static_cast<unsigned>(clientWidth);
	//textureDesc.Height = static_cast<unsigned>(clientHeight);

	//textures.push_back(new PostProcessResource(PPType::UpSample, textureDesc));

	textures.push_back(new PostProcessResource(PPType::ToneMap, std::make_shared<D3D11_TEXTURE2D_DESC>(textureDesc)));

	std::string name;

	for (int i = 0; i < textures.size(); i++)
	{
		ID3D11Texture2D* texture = nullptr;
		hr = device->CreateTexture2D(textures[i]->textureDesc.get(), nullptr, &texture);

		name = "Tex_" + textures[i]->typeToString();
		texture->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.length()), name.c_str());

		hr = device->CreateRenderTargetView(texture, nullptr, textures[i]->rtv.GetAddressOf());

		name = textures[i]->typeToString();
		textures[i]->rtv->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.length()), name.c_str());


		hr = device->CreateShaderResourceView(texture, nullptr, textures[i]->srv.GetAddressOf());

		name = textures[i]->typeToString();
		textures[i]->srv->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.length()), name.c_str());


		textures[i]->spriteRenderer = mySprite->AddComponent<SpriteRenderer>(false);
		textures[i]->spriteRenderer->GetSprite() = new Sprite();

		std::wstring str = EnumToShader(textures[i]->type);

		textures[i]->spriteRenderer->GetMaterial().SetPixelShader(str);
	}
}
