#include "ModelViewer.h"

#include <InputHandler.h>

#include "Application.h"

#include <../Engine/GraphicsEngine/Buffers/FrameBuffer.h>

#include "../Engine/GraphicsEngine/RenderHardwareInterface.h"
#include <Timer.h>

#include "../Engine/GameEngine/Components/Transform.h"

#include "../Engine/GameEngine/ComponentManager.h"

#include <string>

#include "../Engine/GameEngine/DefferedRendering.h"

#include "../Game/GameWorld.h"
#include "../Engine/GameEngine/MainSingleton.h"

#include <TGAFbx.h>
#include "../Engine/GraphicsEngine/DDSTextureLoader11.h"

#include "../Engine/GraphicsEngine/TextureHelper.hpp"

#include "Engine/GraphicsEngine/GraphicsEngine.h"

#include <thread>

#include "../Engine/GameEngine/ModelSprite/ModelFactory.h"

#include "Engine/GameEngine/Components/GameObject.h"

#include "../PhysicsEngine/PhysicsEngine.h"

#include <pix3.h>

using namespace Microsoft::WRL;

ModelViewer::ModelViewer() = default;

bool ModelViewer::Initialize(SIZE aWindowSize, WNDPROC aWindowProcess, LPCWSTR /*aWindowTitle*/)
{
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);

	if (iResult != 0)
	{
		std::cout << "WSAStartup failed: " << iResult << std::endl;
		return false;
	}

	LPCWSTR windowClassName = L"NonGine";

	// First we create our Window Class
	WNDCLASS windowClass = {};
	windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = aWindowProcess;
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.lpszClassName = windowClassName;
	RegisterClass(&windowClass);

	LONG posX = (GetSystemMetrics(SM_CXSCREEN) - aWindowSize.cx) / 2;
	if (posX < 0)
		posX = 0;

	LONG posY = (GetSystemMetrics(SM_CYSCREEN) - aWindowSize.cy) / 2;
	if (posY < 0)
		posY = 0;

	// Then we use the class to create our window
	myMainWindowHandle = CreateWindow(
		windowClassName,                                // Classname
		windowClassName,                                   // Window Title
		WS_OVERLAPPEDWINDOW | WS_POPUP,                 // Flags
		posX,
		posY,
		aWindowSize.cx,
		aWindowSize.cy,
		nullptr,
		nullptr,
		nullptr,
		nullptr
	);

	{
		MVLOG(Log, "Initializing Engine...");

		if (!GraphicsEngine::Get().Init(myMainWindowHandle))
			return false;
	}	
	
	{
		MVLOG(Log, "Initializing Physics Engine...");

		if (!PhysicsEngine::Get().Initialize())
			return false;
	}

	MVLOG(Log, "Ready!");

	ShowWindow(myMainWindowHandle, 5);
	SetForegroundWindow(myMainWindowHandle);

	TGA::FBX::Importer::InitImporter();

	TextureHelper::Init();

	ID3D11ShaderResourceView* none = nullptr;
	TextureHelper::CreateTexture(L"Assets/Shaders/brdfLUT_PS.hlsl", 0, none, MainSingleton::Get()->myLUTTexture);

	std::string meshPath = "Assets/Models/Bone.fbx";


	ImportModelToMemory(meshPath);
	MainSingleton::Get()->mySkeletonBone = CreateModel(meshPath);


	MainSingleton::Get()->myLUTTexture->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Lut texture").length()), "Lut texture");;

	ID3D11Texture2D* cubeTexture = NULL;

	bool separateTextureCubemap = false;

	if (separateTextureCubemap)
	{
		//Description of each face
		D3D11_TEXTURE2D_DESC texDesc = {};

		D3D11_TEXTURE2D_DESC texDesc1 = {};
		//The Shader Resource view description
		D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc = {};

		std::array<std::string, 6> paths =
		{
			"Assets/Textures/posx.dds",
			"Assets/Textures/negx.dds",
			"Assets/Textures/posy.dds",
			"Assets/Textures/negy.dds",
			"Assets/Textures/posz.dds",
			"Assets/Textures/negz.dds",
		};

		Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;


		ID3D11ShaderResourceView* tex[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> outTex[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

		for (int i = 0; i < 6; i++)
		{
			std::wstring pathWString(paths[i].begin(), paths[i].end());

			ID3D11Resource* resource = nullptr;

			DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetRHI()->GetDevice().Get(), pathWString.c_str(), &resource, &tex[i]);

			//TextureHelper::CreateTexture(L"Assets/Shaders/ReWrite_PS.hlsl", 100, tex[i], outTex[i]);
		}
		ID3D11Resource* resourceB = nullptr;
		ID3D11Resource** resource = &resourceB;

		tex[0]->GetResource((ID3D11Resource**)resource);
	
		(*reinterpret_cast<ID3D11Texture2D**>(resource))->GetDesc(&texDesc1);

		texDesc.Width = texDesc1.Width;
		texDesc.Height = texDesc1.Height;
		texDesc.MipLevels = texDesc1.MipLevels;
		texDesc.ArraySize = 6;
		texDesc.Format = texDesc1.Format;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		HRESULT hr = GraphicsEngine::GetRHI()->GetDevice()->CreateTexture2D(&texDesc, nullptr, &cubeTexture);
		assert(SUCCEEDED(hr));

		for (int i = 0; i < 6; i++)
		{
			for (UINT mipLevel = 0; mipLevel < texDesc.MipLevels; ++mipLevel)
			{
				tex[i]->GetResource((ID3D11Resource**)resource);

				GraphicsEngine::GetRHI()->GetContext()->CopySubresourceRegion(cubeTexture, D3D11CalcSubresource(mipLevel, i, texDesc.MipLevels), 0, 0, 0, *resource, mipLevel, nullptr);
			}
		}

		SMViewDesc.Format = texDesc.Format;
		SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		SMViewDesc.TextureCube.MipLevels = texDesc.MipLevels;
		SMViewDesc.TextureCube.MostDetailedMip = 0;

		hr = GraphicsEngine::GetRHI()->GetDevice()->CreateShaderResourceView(cubeTexture, &SMViewDesc, MainSingleton::Get()->skyBoxTexture.GetAddressOf());
		assert(SUCCEEDED(hr));
	}
	else
	{
		DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetRHI()->GetDevice().Get(), L"Assets/Textures/Skansen_Cube.dds", (ID3D11Resource**)cubeTexture, MainSingleton::Get()->skyBoxTexture.GetAddressOf());
	}

	MainSingleton::Get()->skyBoxTexture->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Sky box").length()), "Sky box");

	return true;
}

int ModelViewer::Run()
{
	Transform transform = Transform();

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	GameWorld gameworld(myMainWindowHandle);
	gameworld.SampleSceneCreation();

#ifdef MULTITHREADED
	std::thread renderThread(&DefferedRendering::Render, &deffRendering);
#endif

	while (myIsRunning)
	{
		PIXBeginEvent(PIX_COLOR_INDEX(6), L"Message loop");

		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				myIsRunning = false;
			}

			if (!InputHandler::GetInstance()->myHWnd)
			{
				InputHandler::GetInstance()->myHWnd = msg.hwnd;
			}

			InputHandler::GetInstance()->UpdateEvents(msg.message, msg.wParam, msg.lParam);
		}

		PIXEndEvent();

		static int frame;

		PIXScopedEvent(PIX_COLOR_DEFAULT, L"MainLoop %llu", frame);

		InputHandler::GetInstance()->UpdateInput();
		PIXEndEvent();

#ifdef MULTITHREADED
		if (!deffRendering.myFinishedRendering)
			continue;
#endif
		PIXBeginEvent(PIX_COLOR_INDEX(2), L"Update editor UI");
		gameworld.Begin();
		PIXEndEvent();

		PIXBeginEvent(PIX_COLOR_INDEX(2), L"Clean up");
		GraphicsEngine::Get().CleanUp();
		PIXEndEvent();

		gameworld.EditorUpdate();

		GraphicsEngine::GetRHI()->Present();

		frame++;
	}

	return 0;

}