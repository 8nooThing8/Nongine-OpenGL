#include "../Engine.pch.h"

#include "GraphicsEngine.h"
#include "TextureHelper.hpp"

#include "../GameEngine/Components/SpriteRenderer.h"
#include "../GameEngine/Components/MeshRenderer.h"
#include "../GameEngine/Components/GameObject.h"
#include "../GameEngine/ModelSprite/Sprite.h"

#include "../GameEngine/ShaderResources.h"

#include "Buffers/FrameBuffer.h"
#include "../GameEngine/ModelSprite/ModelFactory.h"

#include "../GameEngine/Components/Transform.h"

void TextureHelper::Init()
{
	if (Instance == nullptr)
	{
		Instance = new TextureHelper();
	}

	Instance->myGO = std::make_shared<GameObject>("aSprite");

	Instance->mySpriteRenderer = Instance->myGO->AddComponent<SpriteRenderer>(false);
	Instance->myMeshRenderer = Instance->myGO->AddComponent<MeshRenderer>(false);
	Instance->mySpriteRenderer->GetSprite() = new Sprite();
	Instance->myMeshRenderer->myMesh = CreateModel("Assets/Models/Cube.fbx");

	Instance->myResources = std::make_shared<ShaderResources>();
}

void TextureHelper::AddTextureResource(const int aSlot, const size_t aNumViews, ID3D11ShaderResourceView* const* aResource)
{
	TextureHelper::Instance->myResources->AddShaderResource(new ShaderResourceItem(aSlot, aNumViews, aResource));
}

void TextureHelper::CreateTexture(const std::wstring& aShaderToUse, const int aSlot, ID3D11ShaderResourceView* aResourceIn, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aResourceOut)
{
	TextureHelper* instance = TextureHelper::Instance;

	D3D11_TEXTURE2D_DESC texDesc = {};

	if (aResourceIn)
	{
		instance->myResources->AddShaderResource(new ShaderResourceItem(aSlot, 1, &aResourceIn));

		ID3D11Resource* r = nullptr;

		ID3D11Resource** resource = &r;

		aResourceIn->GetResource(resource);

		(*(ID3D11Texture2D**)resource)->GetDesc(&texDesc);
	}
	else
	{
		texDesc.Width = 512;
		texDesc.Height = 512;
		texDesc.MipLevels = 0;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
	}

	InitTextures(texDesc);

	instance->mySpriteRenderer->GetMaterial().SetPixelShader(aShaderToUse);

	D3D11_VIEWPORT viewPort = {};

	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = static_cast<FLOAT>(texDesc.Width);
	viewPort.Height = static_cast<FLOAT>(texDesc.Height);

	viewPort.MinDepth = 0;
	viewPort.MaxDepth = 1;


	GraphicsEngine::GetRHI()->ClearRenderTargetView(instance->rtv);

	GraphicsEngine::GetRHI()->SetRenderTargetView(instance->rtv);

	Instance->myGO->transform->scale.x = static_cast<float>(1920);
	Instance->myGO->transform->scale.y = static_cast<float>(1080);

	Instance->myGO->transform->Update(0);

	instance->myResources->SetShaderObjects();

	GraphicsEngine::GetRHI()->GetContext()->RSSetViewports(1, &viewPort);

	instance->mySpriteRenderer->LateUpdateEditor(0);

	TextureHelper::Instance->myResources->CleanUp();

	aResourceOut = Instance->srv;
}

void TextureHelper::CreateTexture3D(const std::wstring& aShaderToUse, const int aSlot, ID3D11ShaderResourceView* aResourceIn, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aResourceOut)
{
	TextureHelper* instance = TextureHelper::Instance;

	instance->myResources->AddShaderResource(new ShaderResourceItem(aSlot, 1, &aResourceIn));

	ID3D11Resource* r = nullptr;

	ID3D11Resource** resource = &r;

	aResourceIn->GetResource(resource);

	D3D11_TEXTURE2D_DESC texDesc = {};

	(*(ID3D11Texture2D**)resource)->GetDesc(&texDesc);

	InitTextures(texDesc);


	instance->myGO->transform->position = GraphicsEngine::Get().frameBufferData.CameraPos + GraphicsEngine::Get().frameBufferData.InvView.GetForward() * 100.f;
	instance->myGO->transform->rotation = CommonUtilities::Vector4<float>(0, 0, 0, 1);
	instance->myGO->transform->scale = CommonUtilities::Vector4<float>(1, 1, 1, 1);

	instance->myGO->transform->Update(0);

	instance->myMeshRenderer->GetMaterial().SetPixelShader(aShaderToUse);

	GraphicsEngine::GetRHI()->SetRenderTargetView(instance->rtv);

	instance->myResources->SetShaderObjects();

	instance->myMeshRenderer->Draw();

	TextureHelper::Instance->myResources->CleanUp();

	aResourceOut = Instance->srv;
}

void TextureHelper::InitTextures(D3D11_TEXTURE2D_DESC aTextureDesc)
{
	Instance->myTextureDesc = aTextureDesc;

	Instance->rtv.ReleaseAndGetAddressOf();
	Instance->srv.ReleaseAndGetAddressOf();

	Microsoft::WRL::ComPtr<ID3D11Device>& device = GraphicsEngine::GetRHI()->GetDevice();

	aTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	aTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	HRESULT result;

	ID3D11Texture2D* texture = nullptr;
	result = device->CreateTexture2D(&aTextureDesc, nullptr, &texture);

	result = device->CreateRenderTargetView(texture, nullptr, Instance->rtv.GetAddressOf());
	result = device->CreateShaderResourceView(texture, nullptr, Instance->srv.GetAddressOf());

	texture->Release();
}
