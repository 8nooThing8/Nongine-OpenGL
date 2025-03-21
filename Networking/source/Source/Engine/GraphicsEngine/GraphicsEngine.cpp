#include "../Engine.pch.h"
#include "GraphicsEngine.h"

#include "../Utilities/Logger/Logger.h"
#include "Shader.h"

#include "Buffers/ObjectBuffer.h"
#include "Buffers/LightBuffer.h"
#include "Buffers/AnimationBuffer.h"
#include "Buffers/DefaultConstants.h"
#include "Buffers/DebugBuffer.h"
#include "Buffers/PostProcessBuffer.h"
#include "Buffers/SpriteEditorBuffer.h"
#include "Buffers/SSAO.h"

#include "../GameEngine/GameCamera.h"

#include "TGAFbx.h"

#include "../GameEngine/Console.h"

#include "../GameEngine/MainSingleton.h"
#include "../GameEngine/Components/MeshRenderer.h"
#include "../GameEngine/GraphicCommands/RenderMeshCommand.h"

#include "../GameEngine/Components/SpriteRenderer.h"
#include "../GameEngine/ModelSprite/Sprite.h"

#include "../GraphicsEngine/ShaderCompiler.h"

#include "../GraphicsEngine/Material/Material.h"
#include "../GraphicsEngine/RenderHardwareInterface.h"

/*
 * GraphicsEngine handles rendering of a scene.
 * Handles culling using whatever cameras it's told to use.
 * Renders onto a user-provided render target and depth.
 */

#ifdef _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(GraphicLog, "GHE", Verbose)
#else
DECLARE_LOG_CATEGORY_WITH_NAME(GraphicLog, "GHE", Error)
#endif

DEFINE_LOG_CATEGORY(GraphicLog);

GraphicsEngine& GraphicsEngine::Get()
{
	static GraphicsEngine myInstance;
	return myInstance;
}

const std::unique_ptr<RenderHardwareInterface>& GraphicsEngine::GetRHI()
{
	return Get().myRHI;
}

void GraphicsEngine::CleanUp()
{
	ID3D11ShaderResourceView* nullViews[] = { nullptr };

	GraphicsEngine::GetRHI()->ClearBackBuffer();

	GraphicsEngine::GetRHI()->GetContext()->PSSetShaderResources(20, 1, nullViews);
	GraphicsEngine::GetRHI()->GetContext()->PSSetShaderResources(21, 1, nullViews);
	GraphicsEngine::GetRHI()->GetContext()->PSSetShaderResources(22, 1, nullViews);
	GraphicsEngine::GetRHI()->GetContext()->PSSetShaderResources(23, 1, nullViews);
	GraphicsEngine::GetRHI()->GetContext()->PSSetShaderResources(24, 1, nullViews);
	GraphicsEngine::GetRHI()->GetContext()->PSSetShaderResources(25, 1, nullViews);

	GraphicsEngine::GetRHI()->GetContext()->PSSetShaderResources(16, 1, nullViews);
	GraphicsEngine::GetRHI()->GetContext()->PSSetShaderResources(64, 1, nullViews);

	MainSingleton::Get()->myComponentManager->CleanUp();
	MainSingleton::Get()->myDefferedCommandList->Reset();
	MainSingleton::Get()->myForwardCommandList->Reset();
	MainSingleton::Get()->myFXCommandList->Reset();

	MainSingleton::Get()->mySkeletonCommandList->Reset();
}

bool GraphicsEngine::Init(HWND aWindowHandle)
{
	myRHI = std::make_unique<RenderHardwareInterface>();
	if (!myRHI->Init(aWindowHandle, true))
	{
		myRHI.reset();
		return false;
	}

	myDefaultPSO.ClearDepthStencil = true;
	myDefaultPSO.ClearRenderTarget = true;

	std::string aVSData;
	Shader shader = Shader(L"", L"");

	Microsoft::WRL::ComPtr<ID3DBlob> blob;

	Shaders::CompileVertexShader(L"Assets/Shaders/Default_VS.hlsl", blob);

	const void* bytecode = blob->GetBufferPointer();
	size_t bytecodeSize = blob->GetBufferSize();

	bool success = myRHI->CreateInputLayout(myDefaultPSO.InputLayout, TGA::FBX::Vertex::InputLayoutDefenition, bytecode, bytecodeSize);

	if (!success)
	{
		PrintC(Console::Error, "Failed to create input layout!");
		return false;
	}

	myDefaultPSO.VertexStride = sizeof(TGA::FBX::Vertex);

	ConstantBuffer frameBuffer;

	if (!myRHI->CreateConstantBuffer("FrameBuffer", sizeof(FrameBuffer), 0, PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER | PIPELINE_STAGE_GEOMETRY_SHADER, frameBuffer))
	{
		PrintC(Console::Error, "Failed to create Framebuffer");
		return false;
	}

	ConstantBuffer objectBuffer;

	if (!myRHI->CreateConstantBuffer("ObjectBuffer", sizeof(ObjectBuffer), 1, PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER, objectBuffer))
	{
		PrintC(Console::Error, "Failed to create Objectbuffer");
		return false;
	}

	ConstantBuffer lightBuffer;

	if (!myRHI->CreateConstantBuffer("LightBuffer", sizeof(LightBuffer), 2, PIPELINE_STAGE_PIXEL_SHADER | PIPELINE_STAGE_VERTEX_SHADER, lightBuffer))
	{
		PrintC(Console::Error, "Failed to create LightBuffer");
		return false;
	}

	ConstantBuffer animationBuffer;

	if (!myRHI->CreateConstantBuffer("AnimationBuffer", sizeof(AnimationBuffer), 3, PIPELINE_STAGE_VERTEX_SHADER, animationBuffer))
	{
		PrintC(Console::Error, "Failed to create AnimationBuffer");
		return false;
	}

	ConstantBuffer defaultConstants;

	if (!myRHI->CreateConstantBuffer("DefaultConstantsBuffer", sizeof(DefaultConstants), 4, PIPELINE_STAGE_PIXEL_SHADER, defaultConstants))
	{
		PrintC(Console::Error, "Failed to create DefaultConstants");
		return false;
	}

	ConstantBuffer debugBuffer;

	if (!myRHI->CreateConstantBuffer("DebugBuffer", sizeof(DebugBuffer), 5, PIPELINE_STAGE_PIXEL_SHADER, debugBuffer))
	{
		PrintC(Console::Error, "Failed to create DebugBuffer");
		return false;
	}

	ConstantBuffer ppBuffer;

	if (!myRHI->CreateConstantBuffer("PostProcessBuffer", sizeof(PostProcessBuffer), 6, PIPELINE_STAGE_PIXEL_SHADER, ppBuffer))
	{
		PrintC(Console::Error, "Failed to create PostProcessBuffer");
		return false;
	}

	ConstantBuffer spriteEditorBuffer;

	if (!myRHI->CreateConstantBuffer("SpriteEditorBuffer", sizeof(SpriteEditorBuffer), 7, PIPELINE_STAGE_PIXEL_SHADER, spriteEditorBuffer))
	{
		PrintC(Console::Error, "Failed to create SpriteEditorBuffer");
		return false;
	}

	ConstantBuffer ssao;

	if (!myRHI->CreateConstantBuffer("ssao", sizeof(SSAO), 8, PIPELINE_STAGE_PIXEL_SHADER, ssao))
	{
		PrintC(Console::Error, "Failed to create ConstantBuffer");
		return false;
	}

	D3D11_BLEND_DESC blendState;
	ZeroMemory(&blendState, sizeof(D3D11_BLEND_DESC));
	blendState.RenderTarget[0].BlendEnable = true;
	blendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;;
	blendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;


	if (FAILED(myRHI->myDevice->CreateBlendState(&blendState, myBlendstate.GetAddressOf())))
	{
		PrintC(Console::Error, "Failed to create Blendstate");
		return false;
	}

	myBlendstate->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Default").length()), "Default");

	myConstantBuffer.emplace(ConstantBufferType::ObjectBuffer, std::move(objectBuffer));
	myConstantBuffer.emplace(ConstantBufferType::FrameBuffer, std::move(frameBuffer));
	myConstantBuffer.emplace(ConstantBufferType::LightBuffer, std::move(lightBuffer));
	myConstantBuffer.emplace(ConstantBufferType::AnimationBuffer, std::move(animationBuffer));
	myConstantBuffer.emplace(ConstantBufferType::Defaultconstants, std::move(defaultConstants));
	myConstantBuffer.emplace(ConstantBufferType::DebugBuffer, std::move(debugBuffer));
	myConstantBuffer.emplace(ConstantBufferType::PostProcess, std::move(ppBuffer));
	myConstantBuffer.emplace(ConstantBufferType::SpriteEditor, std::move(spriteEditorBuffer));
	myConstantBuffer.emplace(ConstantBufferType::SSAO, std::move(ssao));

	myCurrentPSO = myDefaultPSO;

	std::vector<CommonUtilities::Matrix4x4<float>> myInstancedModels;

	myInstancedModels.reserve(65000);

	myInstancedModels = { CommonUtilities::Matrix4x4<float>() };


	GraphicsEngine::GetRHI()->CreateVertexBuffer("Instance buffer", myInstancedModels, myInstanceBuffer, true);

	return true;
}

void GraphicsEngine::GEChangePipelineState(const PipelineStateObject& aNewPSO)
{
	myRHI->ChangePipelineState(aNewPSO, myCurrentPSO);
	myCurrentPSO = aNewPSO;
}

void GraphicsEngine::RenderMesh(TGA::FBX::Mesh* aMesh, Material& aMaterial, Microsoft::WRL::ComPtr<ID3D11PixelShader> shaderOverride) const
{
	if (aMesh)
	{
		myRHI->SetVertexBuffer(aMesh->myVertexBuffer, sizeof(TGA::FBX::Vertex), 0);
		myRHI->SetIndexBuffer(aMesh->myIndexBuffer);

		myRHI->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		if (shaderOverride && aMaterial.GetPSShader()->myName != L"Assets/Shaders/ReflectionProbe_PS.hlsl")
		{
			Material material;

			material.myPSShader = std::make_shared<Shader>();

			material.myPSShader->myshader = shaderOverride;

			GetRHI()->SetShaders(*aMaterial.GetVXShader(), shaderOverride);
		}
		else
		{
			GetRHI()->SetShaders(*aMaterial.GetVXShader(), *aMaterial.GetPSShader());
		}

		for (int i = 0; i < aMesh->Elements.size(); i++)
		{
			const size_t indexOffset = (i == 0) ? 0 : aMesh->Elements[i - 1].Indices.size();
			const size_t vertexOffset = (i == 0) ? 0 : aMesh->Elements[i - 1].Vertices.size();

			myRHI->DrawIndexed(static_cast<unsigned>(indexOffset), static_cast<unsigned>(aMesh->Elements[i].Indices.size()), static_cast<unsigned>(vertexOffset), aMaterial);
		}
	}
}

void GraphicsEngine::RenderMeshes(TGA::FBX::Mesh* aMesh, Material& aMaterial, const std::vector<CommonUtilities::Matrix4x4<float>>& aTransforms)
{
	std::vector<unsigned> vertexStrides = { static_cast<unsigned>(sizeof(TGA::FBX::Vertex)), static_cast<unsigned>(sizeof(CommonUtilities::Matrix4x4<float>)) };
	std::vector<unsigned> vertexOffset = { 0, 0 };



	std::vector<ID3D11Buffer*> buffers = { aMesh->myVertexBuffer.Get(), myInstanceBuffer.Get() };

	if (aMesh)
	{
		myRHI->SetVertexBuffers(buffers, vertexStrides, vertexOffset);
		myRHI->SetIndexBuffer(aMesh->myIndexBuffer);

		myRHI->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


		myRHI->SetShaders(*aMaterial.GetVXShader(), *aMaterial.GetPSShader());

		for (size_t i = 0; i < aMesh->Elements.size(); i++)
		{
			const size_t indexOffsets = (i == 0) ? 0 : aMesh->Elements[i - 1].Indices.size();
			const size_t vertexOffsets = (i == 0) ? 0 : aMesh->Elements[i - 1].Vertices.size();

			myRHI->DrawInstanced(static_cast<unsigned>(aMesh->Elements[i].Indices.size()), static_cast<unsigned>(indexOffsets), static_cast<unsigned>(vertexOffsets), static_cast<unsigned>(aTransforms.size()), aMaterial);
		}
	}
}

void GraphicsEngine::Rendersprite(Sprite* aSprite, Material& aMaterial) const
{
	//float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	//UINT sampleMask = 0xffffffff;

	//RenderHardwareInterface::myContext->OMSetBlendState(GraphicsEngine::Get().myBlendstate.Get(), 0, sampleMask);

	if (aSprite)
	{
		myRHI->SetVertexBuffer(aSprite->myVertexBuffer, sizeof(Vert), 0);
		myRHI->SetIndexBuffer(aSprite->myIndexBuffer);

		myRHI->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		GraphicsEngine::GetRHI().get()->SetShaders(*aMaterial.GetVXShader(), *aMaterial.GetPSShader());

		myRHI->DrawIndexedSprite(0, 6, 0, aSprite);
	}
}

void GraphicsEngine::DrawMesh(MeshRenderer* aMeshrenderer)
{
	if (aMeshrenderer->myDeffered)
	{
		MainSingleton::Get()->myDefferedCommandList->Enqueue<RenderMeshCommand>(aMeshrenderer);
		MainSingleton::Get()->myDefferedCommandList->myObjects.emplace_back(aMeshrenderer->gameObject);
	}
	else
	{
		MainSingleton::Get()->myForwardCommandList->Enqueue<RenderMeshCommand>(aMeshrenderer);
		MainSingleton::Get()->myForwardCommandList->myObjects.emplace_back(aMeshrenderer->gameObject);
	}
}

void GraphicsEngine::UpdateRender(float /*aDeltaTime*/)
{
	frameBufferData.Projection = GameCamera::main->GetProjectionMatrix();

	CommonUtilities::Matrix4x4<float> transform;

	float camRotX = GameCamera::main->rotation.x;
	float camRotY = GameCamera::main->rotation.y;

	transform.SetPosition(-GameCamera::main->position);

	transform *= CommonUtilities::Matrix4x4<float>::CreateRotationAroundY(CommonUtilities::DegToRad(camRotY)).GetInverse();
	transform *= CommonUtilities::Matrix4x4<float>::CreateRotationAroundX(CommonUtilities::DegToRad(camRotX));

	frameBufferData.InvView = transform;

	frameBufferData.CamRot = GameCamera::main->rotation;

	frameBufferData.CameraPos = GameCamera::main->position;

	frameBufferData.aspectRatio = GraphicsEngine::Get().myGameResolution.x / GraphicsEngine::Get().myGameResolution.y;

	UpdateAndSetConstantBuffer(ConstantBufferType::FrameBuffer, frameBufferData);
}

template<class BufferData>
FORCEINLINE bool GraphicsEngine::UpdateAndSetConstantBuffer(const ConstantBufferType aBufferType, const BufferData& aDataBlock)
{
	if (!myConstantBuffer.contains(aBufferType))
	{
		return false;
	}

	ConstantBuffer& buffer = myConstantBuffer.at(aBufferType);

	if (!myRHI->UpdateConstantBuffer(buffer, aDataBlock))
	{
		return false;
	}

	myRHI->SetConstantBuffer(buffer);
	return true;
}

GraphicsEngine::GraphicsEngine() = default;
GraphicsEngine::~GraphicsEngine() = default;
