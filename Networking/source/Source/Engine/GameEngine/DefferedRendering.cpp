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

using namespace CommonUtilities;

void DefferedRendering::Render()
{
#ifdef MULTITHREADED
	while (true)
	{
		if (myFinishedRendering)
			continue;
#endif


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

		GraphicsEngine::GetRHI()->SetDefferedBufferRT();

		GraphicsEngine::GetRHI()->GetContext()->OMSetBlendState(myBlendstate.Get(), blendfactor.data(), samplerMask);

		GraphicsEngine::GetRHI()->GetContext()->PSSetShaderResources(33, 1, MainSingleton::Get()->myLUTTexture.GetAddressOf());
		GraphicsEngine::GetRHI()->GetContext()->PSSetSamplers(2, 1, myLutSampler.GetAddressOf());

		MainSingleton::Get()->myDefferedCommandList->Execute();

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

		GraphicsEngine::GetRHI()->GetContext()->OMSetBlendState(myBlendstate.Get(), blendfactor.data(), samplerMask);

		MainSingleton::Get()->shaderResources->SetShaderObjects();

		myPostProcessing.textures[6]->spriteRenderer->LateUpdateEditor(0);

		PIXEndEvent();

		GraphicsEngine::GetRHI()->SetPostProcessBufferRT(false);

		MainSingleton::Get()->shaderResources->SetShaderObjects();

		GraphicsEngine::GetRHI()->GetContext()->PSSetShaderResources(25, 1, myPostProcessing.textures[6]->srv.GetAddressOf());

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

		if (MainSingleton::Get()->myIsPlaying)
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
	blendState.RenderTarget[0].BlendEnable = false;
	blendState.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
	blendState.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// Create the blend state
	if (FAILED(GraphicsEngine::GetRHI()->myDevice->CreateBlendState(&blendState, myBlendstate.GetAddressOf())))
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
