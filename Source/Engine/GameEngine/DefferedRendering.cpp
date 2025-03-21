#include "../Engine.pch.h"

#include <array>

#include "DefferedRendering.h"

#include "Components/GameObject.h"
#include "Components/MeshRenderer.h"
#include "Components/SpriteRenderer.h"
#include "InspectorCamera.h"

#include "../GraphicsEngine/GraphicsEngine.h"

#include "../GameEngine/ModelSprite/Sprite.h"

#include "../Engine/GameEngine/ShaderResources.h"
#include "ModelSprite/ModelFactory.h"

#include "../GraphicsEngine/Shader.h"

#include "../GameEngine/Components/InstanceComponent.h"

#include "../GraphicsEngine/Buffers/DebugBuffer.h"

using namespace CommonUtilities;

void DefferedRendering::Render()
{
#ifdef MULTITHREADED
	while (true)
	{
		if (myFinishedRendering)
			continue;
#endif

		ID3D11ShaderResourceView* nullSRV = nullptr;
		GraphicsEngine::GetRHI()->GetContext()->PSSetShaderResources(50, 1, &nullSRV);

		CU::Vector4f rgba(0.3f, 0.3f, 0.3f, 1.f);
		GraphicsEngine::GetRHI()->GetContext()->ClearRenderTargetView(rtv.Get(), &rgba[0]);

		ID3D11ShaderResourceView* nullViews[] = { nullptr };
		GraphicsEngine::GetRHI()->GetContext()->PSSetShaderResources(80, 1, nullViews);


		const std::array<float, 4> blendfactor = { 0, 0, 0, 0 };
		constexpr unsigned samplerMask = 0xffffffff;


		Vector2<float> resolution = GraphicsEngine::Get().myGameResolution;

		aFullScreensprite->transform->scale = Vector4<float>(1920, 1080, 1, 1);
		aFullScreensprite->transform->Update(0);

		PIXBeginEvent(PIX_COLOR_INDEX(2), L"Clear buffers");

		GraphicsEngine::GetRHI()->ClearDefferedBuffer();
		GraphicsEngine::GetRHI()->ClearPPBuffer();
		GraphicsEngine::GetRHI()->ClearSceneBuffer();

		PIXEndEvent();

		PIXBeginEvent(PIX_COLOR_INDEX(2), L"Deffered render");

		GraphicsEngine::GetRHI()->SetDefferedBufferRT(rtv.Get());

		GraphicsEngine::GetRHI()->GetContext()->OMSetDepthStencilState(nullptr, 0);
		//GraphicsEngine::GetRHI()->GetContext()->OMSetDepthStencilState(nullptr, 0);


		GraphicsEngine::GetRHI()->GetContext()->PSSetShaderResources(33, 1, MainSingleton::Get()->myLUTTexture.GetAddressOf());
		GraphicsEngine::GetRHI()->GetContext()->PSSetSamplers(2, 1, myLutSampler.GetAddressOf());

		//MainSingleton::Get()->myDefferedCommandList->Execute();

		//MainSingleton::Get()->myComponentManager->RenderMeshesOnLayer(0);

		RenderAllMeshes();

		GraphicsEngine::GetRHI()->GetContext()->OMSetDepthStencilState(nullptr, 0);

		auto& instanceComponent = MainSingleton::Get()->instanceComponent;

		if (instanceComponent)
		{
			ObjectBuffer objectBuffer = instanceComponent->gameObject->transform->GetObjBuffer();
			objectBuffer.albedo = CommonUtilities::Vector4<float>(1, 1, 1, 1);

			objectBuffer.isInstanced = true;

			GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objectBuffer);

			GraphicsEngine::Get().RenderMeshes(instanceComponent->instancedMesh, instanceComponent->myInstanceMaterial, instanceComponent->myInstancedModels);
		}

		std::array<ID3D11ShaderResourceView*, 5> resourceViews =
		{
			GraphicsEngine::GetRHI()->myDefferedAlbedoSRV.Get(),
			GraphicsEngine::GetRHI()->myDefferedNormalSRV.Get(),
			GraphicsEngine::GetRHI()->myDefferedPositionSRV.Get(),
			GraphicsEngine::GetRHI()->myDefferedMaterialSRV.Get(),
			GraphicsEngine::GetRHI()->myDefferedPixelPickerSRV.Get()
		};

		MainSingleton::Get()->shaderResources->AddShaderResource(new ShaderResourceItem(20, 5, resourceViews.data()));
		MainSingleton::Get()->shaderResources->AddShaderResource(new ShaderResourceItem(32, 1, MainSingleton::Get()->skyBoxTexture.GetAddressOf()));

		PIXBeginEvent(PIX_COLOR_INDEX(2), L"SSAO render");

		GraphicsEngine::GetRHI()->SetRenderTargetView(myPostProcessing.textures[6]->rtv);

		GraphicsEngine::GetRHI()->GetContext()->PSSetShaderResources(50, 1, rtvSRV.GetAddressOf());

		GraphicsEngine::GetRHI()->GetContext()->OMSetBlendState(myNoBlendstate.Get(), blendfactor.data(), samplerMask);

		MainSingleton::Get()->shaderResources->SetShaderObjects();

		myPostProcessing.textures[6]->spriteRenderer->LateUpdateEditor(0);

		PIXEndEvent();

		GraphicsEngine::GetRHI()->SetPostProcessBufferRT(false);

		MainSingleton::Get()->shaderResources->SetShaderObjects();

		GraphicsEngine::GetRHI()->GetContext()->PSSetShaderResources(25, 1, myPostProcessing.textures[6]->rtvSRV.GetAddressOf());

		MainSingleton::Get()->mySkyBox->transform->position = GraphicsEngine::Get().frameBufferData.CameraPos;
		MainSingleton::Get()->mySkyBox->transform->Update(0);
		MainSingleton::Get()->mySkyBoxMeshRend->Draw();

		aFullScreenspriteRenderer->LateUpdateEditor(0);

		PIXEndEvent();

		GraphicsEngine::GetRHI()->SetPostProcessBufferRT(true);

		PIXBeginEvent(PIX_COLOR_INDEX(2), L"RenderTrail");

		MainSingleton::Get()->myFXCommandList->Execute();

		GraphicsEngine::GetRHI()->GetContext()->RSSetState(GraphicsEngine::GetRHI()->rsPt.Get());

		PIXEndEvent();

		PIXBeginEvent(PIX_COLOR_INDEX(2), L"Post process preparation");

		GraphicsEngine::GetRHI()->GetContext()->OMSetBlendState(GraphicsEngine::Get().myBlendstate.Get(), blendfactor.data(), samplerMask);

		PIXBeginEvent(PIX_COLOR_INDEX(2), L"Render forward commands");

		MainSingleton::Get()->myForwardCommandList->Execute();

		PIXEndEvent();

		GraphicsEngine::GetRHI()->GetContext()->RSSetState(GraphicsEngine::GetRHI()->rsNoCulling.Get());

		PIXEndEvent();

		PIXBeginEvent(PIX_COLOR_INDEX(2), L"Full post process");

		aPostprocessSprite->GetSprite()->aTexture = myPostProcessing.Render(GraphicsEngine::GetRHI()->myPostProcessSRV.Get());
		//aPostprocessSprite->GetSprite()->aTexture = myPostProcessing.Render(myPostProcessing.textures[6]->srv.Get());

		GraphicsEngine::GetRHI()->myPostProcessSRV->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Post process").length()), "Post process");

		MainSingleton::Get()->mySkeletonCommandList->Execute();

		if (MainSingleton::Get()->fullscreenPlaying)
		{
			GraphicsEngine::GetRHI()->SetBackBufferRT();
		}
		else
		{
			GraphicsEngine::GetRHI()->SetSceneBufferRT(false);
		}


		aPostprocessSprite->LateUpdateEditor(0);

		PIXEndEvent();

		MainSingleton::Get()->myComponentManager->RenderSprite();


#ifdef MULTITHREADED
		myFinishedRendering = true;
	}
#endif
}

void DefferedRendering::Init(const HWND aHWND)
{
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = false;

	GraphicsEngine::GetRHI()->GetDevice()->CreateRasterizerState(&rasterizerDesc, &myHoleState);


	D3D11_DEPTH_STENCIL_DESC holeStencilDesc = {};
	holeStencilDesc.DepthEnable = true; // Enable depth testing
	holeStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // Don't write to depth buffer
	holeStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS; // Standard depth test

	//holeStencilDesc.StencilEnable = true; // Enable stencil testing
	//holeStencilDesc.StencilReadMask = 0xFF; // Read full stencil buffer
	//holeStencilDesc.StencilWriteMask = 0xFF; // Write full stencil buffer

	//holeStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//holeStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	//holeStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//holeStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL; // Discard if stencil == 1

	//holeStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//holeStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	//holeStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//holeStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL; // Discard if stencil == 1

	GraphicsEngine::GetRHI()->GetDevice()->CreateDepthStencilState(&holeStencilDesc, &holeStencilState);


	D3D11_DEPTH_STENCIL_DESC groundStencilDesc = {};
	groundStencilDesc.DepthEnable = true; // Enable depth testing
	groundStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // Write to depth buffer
	groundStencilDesc.DepthFunc = D3D11_COMPARISON_NOT_EQUAL; // Standard depth test

	groundStencilDesc.StencilEnable = true; // Enable stencil testing
	groundStencilDesc.StencilReadMask = 0xFF; // Read full stencil buffer
	groundStencilDesc.StencilWriteMask = 0xFF; // Don't write to stencil buffer

	// Stencil Operations (Discard pixels where stencil == 1)
	groundStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	groundStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	groundStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	groundStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL; // Discard if stencil == 1

	groundStencilDesc.BackFace = groundStencilDesc.FrontFace; // Same for back faces

	//GraphicsEngine::GetRHI()->GetDevice()->CreateDepthStencilState(&groundStencilDesc, &depthStencilState);

	{
		myViewPort.TopLeftX = 0;
		myViewPort.TopLeftY = 0;
		myViewPort.Width = 1920;
		myViewPort.Height = 1080;

		myViewPort.MinDepth = 0;
		myViewPort.MaxDepth = 1;
	}

	RECT clientRect = {};
	GetClientRect(aHWND, &clientRect);

	const float clientWidth = static_cast<float>(clientRect.right - clientRect.left);
	const float clientHeight = static_cast<float>(clientRect.bottom - clientRect.top);

	D3D11_TEXTURE2D_DESC textureDesc = {};

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};

	rtvDesc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	textureDesc.Width = static_cast<int>(clientWidth);
	textureDesc.Height = static_cast<int>(clientHeight);
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	ID3D11Texture2D* texture = nullptr;
	GraphicsEngine::GetRHI()->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &texture);

	GraphicsEngine::GetRHI()->GetDevice()->CreateRenderTargetView(texture, &rtvDesc, rtv.GetAddressOf());

	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc{};
	srDesc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;

	GraphicsEngine::GetRHI()->GetDevice()->CreateShaderResourceView(texture, &srDesc, rtvSRV.GetAddressOf());

	rtvSRV->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Hole RTV").length()), "Hole RTV");

	//std::shared_ptr<Shader> pixelShader = std::make_shared<Shader>(L"", L"");
	//std::shared_ptr<Shader> vertexShader = std::make_shared<Shader>(L"", L"");

	//pixelShader->SetShaderNamePath(L"InstancedPixelShader", L"Assets/Shaders/Deffered_PS.hlsl");
	//vertexShader->SetShaderNamePath(L"InstancedVertexShader", L"Assets/Shaders/Default_VS.hlsl");

	//Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShaderd3d11 = Shaders::CompilePixelShader(pixelShader->GetShaderPath());
	//pixelShader->SetShader(pixelShaderd3d11);

	//Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShaderd3d11 = Shaders::CompileVertexShader(vertexShader->GetShaderPath());
	//vertexShader->SetShader(vertexShaderd3d11);



	aFullScreensprite = std::make_shared<GameObject>("Deferred render sprite");

	aFullScreenspriteRenderer = aFullScreensprite->AddComponent<SpriteRenderer>(false);
	aFullScreenspriteRenderer->GetMaterial().SetPixelShader(L"Assets/Shaders/DefferedFullscreen_PS.hlsl");

	aFullScreenspriteRenderer->GetSprite() = new Sprite();

	aPostprocessSprite = aFullScreensprite->AddComponent<SpriteRenderer>(false);
	aPostprocessSprite->GetMaterial().SetPixelShader(L"Assets/Shaders/Sprite_UnLit_PS.hlsl");

	aPostprocessSprite->GetSprite() = new Sprite();

	D3D11_BLEND_DESC blendState;
	ZeroMemory(&blendState, sizeof(D3D11_BLEND_DESC));

	// Disable blending for the render target
	blendState.RenderTarget[0].BlendEnable = true;
	blendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// Create the blend state
	if (FAILED(GraphicsEngine::GetRHI()->myDevice->CreateBlendState(&blendState, myBlendstate.GetAddressOf())))
	{
		PrintC(Console::Error, "Failed to create Blendstate");
	}

	ZeroMemory(&blendState, sizeof(D3D11_BLEND_DESC));

	// Disable blending for the render target
	blendState.RenderTarget[0].BlendEnable = false;
	blendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// Create the blend state
	if (FAILED(GraphicsEngine::GetRHI()->myDevice->CreateBlendState(&blendState, myNoBlendstate.GetAddressOf())))
	{
		PrintC(Console::Error, "Failed to create Blendstate");
	}

	myBlendstate->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Deffered").length()), "Deffered");

	D3D11_SAMPLER_DESC lutSamplerDesc = {};
	lutSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	lutSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	lutSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	lutSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	lutSamplerDesc.MipLODBias = 0.0f;
	lutSamplerDesc.MaxAnisotropy = 1;
	lutSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	lutSamplerDesc.BorderColor[0] = 0.0f;
	lutSamplerDesc.BorderColor[1] = 0.0f;
	lutSamplerDesc.BorderColor[2] = 0.0f;
	lutSamplerDesc.BorderColor[3] = 0.0f;
	lutSamplerDesc.MinLOD = 0;
	lutSamplerDesc.MaxLOD = 0;

	GraphicsEngine::GetRHI()->myDevice->CreateSamplerState(&lutSamplerDesc, myLutSampler.GetAddressOf());

	myLutSampler->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Lut sampler").length()), "Lut sampler");

	myPostProcessing.Init(aHWND);
}

void DefferedRendering::RenderAllMeshes()
{
	DebugBuffer debugBuffer;

	const std::array<float, 4> blendfactor = { 0, 0, 0, 0 };
	constexpr unsigned samplerMask = 0xffffffff;

	myLayerMeshRenderer.clear();

	for (auto& layerIndex : layers)
	{
		auto gameObjects = MainSingleton::Get()->myComponentManager->GetGameObjectsOnLayer(layerIndex);

		const CU::Vector3f cameraPosition = GameCamera::main->position.ToVector3();

		for (auto& gameObject : gameObjects)
		{
			const CU::Vector3f gameobjectPosition = gameObject->transform->position.ToVector3();

			const float distanceGameObjectToCamera = (cameraPosition - gameobjectPosition).LengthSqr();

			myLayerMeshRenderer.push_back({ distanceGameObjectToCamera,
				[&, index = layerIndex]() {
					{
						GraphicsEngine::GetRHI()->GetContext()->OMSetBlendState(myBlendstate.Get(), blendfactor.data(), samplerMask);

						GraphicsEngine::GetRHI()->GetContext()->RSSetState(myHoleState);

						GraphicsEngine::GetRHI()->GetContext()->OMSetDepthStencilState(holeStencilState.Get(), 2);
						debugBuffer.isMask = 1;
						GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::DebugBuffer, debugBuffer);
						MainSingleton::Get()->myComponentManager->RenderRemoveMeshesLayer(index);
					}

					{
						GraphicsEngine::GetRHI()->GetContext()->OMSetBlendState(myNoBlendstate.Get(), blendfactor.data(), samplerMask);

						GraphicsEngine::GetRHI()->GetContext()->RSSetState(GraphicsEngine::GetRHI()->rsPt.Get());
						GraphicsEngine::GetRHI()->GetContext()->OMSetDepthStencilState(depthStencilState.Get(), 1);
						debugBuffer.isMask = 0;
						GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::DebugBuffer, debugBuffer);
						MainSingleton::Get()->myComponentManager->RenderMeshesOnLayer(index);
					}
				} });
		}
	}

	std::sort(myLayerMeshRenderer.begin(), myLayerMeshRenderer.end(), [&](std::pair<float, std::function<void()>> object0, std::pair<float, std::function<void()>> object1)
		{
			return object0.first > object1.first;
		});

	for (auto& layerMesh : myLayerMeshRenderer)
	{
		layerMesh.second();
	}
}