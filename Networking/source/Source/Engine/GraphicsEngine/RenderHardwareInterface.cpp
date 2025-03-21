#include "../Engine.pch.h"
#include "RenderHardwareInterface.h"

#include "../Utilities/Logger/Logger.h"

#include <vector>
#include "../Utilities/StringHelpers.h"
#include "Texture.h"

#include "Shader.h"

#include <d3dcompiler.h>

#include "Buffers/ConstantBuffer.h"

#include <assert.h>

#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"

#include "../GameEngine/Console.h"
#include "../GraphicsEngine/Buffers/DefaultConstants.h"

#include "Material/Material.h"

#include "../GameEngine/ModelSprite/Sprite.h"

#include <array>

#include "../GameEngine/MainSingleton.h"

#include "dxgiformat.h"

#include "../Engine/GameEngine/ShaderResources.h"

struct Mesh;

#ifdef _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(RhiLog, "RHI", Verbose)
#else
DECLARE_LOG_CATEGORY_WITH_NAME(RhiLog, "RHI", Error)
#endif

DEFINE_LOG_CATEGORY(RhiLog);

using namespace Microsoft::WRL;

void DisableD3D11Warning(ID3D11Device* device, const D3D11_MESSAGE_ID messageId)
{
	ID3D11InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device->QueryInterface(__uuidof(ID3D11InfoQueue), reinterpret_cast<void**>(&infoQueue)))) {
		D3D11_MESSAGE_ID hideMessages[] = { messageId };
		D3D11_INFO_QUEUE_FILTER filter = {};
		filter.DenyList.NumIDs = _countof(hideMessages);
		filter.DenyList.pIDList = hideMessages;
		infoQueue->AddStorageFilterEntries(&filter);
		infoQueue->Release();
	}
	else
	{
		std::cerr << "Failed to get ID3D11InfoQueue interface. Make sure the debug layer is enabled.\n";
	}
}

bool RenderHardwareInterface::Init(HWND aHWND, bool aEnableDebug)
{
	HRESULT result = E_FAIL;

	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr))
		return false;

	ComPtr<IDXGIFactory> dxFactory;

	result = CreateDXGIFactory(__uuidof(IDXGIFactory), &dxFactory);

	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create DX Factory!");
		return false;
	}

	LOG(RhiLog, Log, "Initilizing RHI...");

	ComPtr<IDXGIAdapter> tempAdapter;

	std::vector<ComPtr<IDXGIAdapter>> adapters;

	unsigned adaptercount = 0;
	while (dxFactory->EnumAdapters(adaptercount, &tempAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		adapters.push_back(tempAdapter);
		adaptercount++;
	}

	ComPtr<IDXGIAdapter> selectedAdapter;
	size_t selectedAdapterVRAM = 0;
	DXGI_ADAPTER_DESC selectedAdapterDesc = {};

	for (const auto& adapter : adapters)
	{
		DXGI_ADAPTER_DESC currentDesc = {};
		adapter->GetDesc(&currentDesc);

		if (currentDesc.DedicatedVideoMemory > selectedAdapterVRAM)
		{
			selectedAdapterVRAM = currentDesc.DedicatedVideoMemory;
			selectedAdapter = adapter;
			selectedAdapterDesc = currentDesc;
		}
	}

	const wchar_t* wideAdapterName = selectedAdapterDesc.Description;

	std::string adapterName = str::wide_to_utf8(wideAdapterName);

	LOG(RhiLog, Log, "Selected adapter is {}", adapterName);
	constexpr size_t megabyte = (1024ULL * 1024ULL);

	if (selectedAdapterVRAM > megabyte)
	{
		selectedAdapterVRAM /= megabyte;
	}

	LOG(RhiLog, Log, "* VRAM: {} MB", selectedAdapterVRAM);

	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	// Define the ordering of feature levels that Direct3D attempts to create.
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_1
	};

	result = D3D11CreateDevice(
		selectedAdapter.Get(),
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL,
		aEnableDebug ? D3D11_CREATE_DEVICE_DEBUG : 0,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&device,
		NULL,
		&context
	);

	if (FAILED(result))
	{
		PrintC(Console::Error, "Failed to initilize DirectX!");
	}

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};

	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = aHWND;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
	swapChainDesc.Windowed = true;

	ComPtr<IDXGISwapChain> swapChain;
	result = dxFactory->CreateSwapChain(device.Get(), &swapChainDesc, &swapChain);

	if (FAILED(result))
	{
		PrintC(Console::Error, "Failed to create swapchain!");
		return false;
	}

	swapChain->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Default").length()), "Default");

	ComPtr<ID3D11Texture2D> backBufferTexture;

	result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBufferTexture);

	myBackBuffer = std::make_shared<Texture>();
	result = device->CreateRenderTargetView(backBufferTexture.Get(), nullptr, myBackBuffer->myRTV.GetAddressOf());

	myBackBuffer->myRTV->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Back buffer").length()), "Back buffer");

	RECT clientRect = {};
	GetClientRect(aHWND, &clientRect);

	const float clientWidth = static_cast<float>(clientRect.right - clientRect.left);
	const float clientHeight = static_cast<float>(clientRect.bottom - clientRect.top);

	{
		// Create the texture description
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = static_cast<unsigned>(clientWidth);
		textureDesc.Height = static_cast<unsigned>(clientHeight);
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		// forward rendering RTV
		{
			ID3D11Texture2D* texture = nullptr;
			hr = device->CreateTexture2D(&textureDesc, nullptr, &texture);

			if (FAILED(result))
			{
				PrintC(Console::Error, "Failed to retrieve backbuffer!");
				return false;
			}

			mySceneBuffer = std::make_shared<Texture>();
			result = device->CreateRenderTargetView(texture, nullptr, mySceneBuffer->myRTV.GetAddressOf());

			if (FAILED(result))
			{
				PrintC(Console::Error, "Failed to create render target view!");
				return false;
			}

			mySceneBuffer->myRTV->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Scene view").length()), "Scene view");

			result = device->CreateShaderResourceView(texture, nullptr, mySceneSRV.GetAddressOf());

			if (FAILED(result))
			{
				PrintC(Console::Error, "Failed to create sceneSRV");
				return false;
			}

			mySceneSRV->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Scene view").length()), "Scene view");
		}

		// Creating Post processing
		{
			textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

			ID3D11Texture2D* texture = nullptr;
			hr = device->CreateTexture2D(&textureDesc, nullptr, &texture);

			if (FAILED(result))
			{
				PrintC(Console::Error, "Failed to retrieve backbuffer!");
				return false;
			}

			myPostProcessBuffer = std::make_shared<Texture>();
			result = device->CreateRenderTargetView(texture, nullptr, myPostProcessBuffer->myRTV.GetAddressOf());

			if (FAILED(result))
			{
				PrintC(Console::Error, "Failed to create render target view!");
				return false;
			}

			myPostProcessBuffer->myRTV->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Post process").length()), "Post process");

			result = device->CreateShaderResourceView(texture, nullptr, myPostProcessSRV.GetAddressOf());

			if (FAILED(result))
			{
				PrintC(Console::Error, "Failed to create sceneSRV");
				return false;
			}

			myPostProcessSRV->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Post process").length()), "Post process");
		}

		// Creating Albedo Image
		{
			textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

			ID3D11Texture2D* texture = nullptr;
			hr = device->CreateTexture2D(&textureDesc, nullptr, &texture);

			if (FAILED(result))
			{
				PrintC(Console::Error, "Failed to retrieve backbuffer!");
				return false;
			}

			myDefferedAlbedoBuffer = std::make_shared<Texture>();
			result = device->CreateRenderTargetView(texture, nullptr, myDefferedAlbedoBuffer->myRTV.GetAddressOf());

			if (FAILED(result))
			{
				PrintC(Console::Error, "Failed to create render target view!");
				return false;
			}

			myDefferedAlbedoBuffer->myRTV->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Deffered Albedo").length()), "Deffered Albedo");

			result = device->CreateShaderResourceView(texture, nullptr, myDefferedAlbedoSRV.GetAddressOf());

			if (FAILED(result))
			{
				PrintC(Console::Error, "Failed to create sceneSRV");
				return false;
			}

			myDefferedAlbedoSRV->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Deffered Albedo").length()), "Deffered Albedo");
		}	

		// Creating Pixel normal vectorImage
		{
			textureDesc.Format = DXGI_FORMAT_R16G16B16A16_SNORM;

			ID3D11Texture2D* texture = nullptr;
			hr = device->CreateTexture2D(&textureDesc, nullptr, &texture);

			if (FAILED(result))
			{
				PrintC(Console::Error, "Failed to retrieve backbuffer!");
				return false;
			}

			myDefferedNormalBuffer = std::make_shared<Texture>();
			result = device->CreateRenderTargetView(texture, nullptr, myDefferedNormalBuffer->myRTV.GetAddressOf());

			if (FAILED(result))
			{
				PrintC(Console::Error, "Failed to create render target view!");
				return false;
			}

			myDefferedNormalBuffer->myRTV->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Deffered Normal").length()), "Deffered Normal");

			result = device->CreateShaderResourceView(texture, nullptr, myDefferedNormalSRV.GetAddressOf());

			if (FAILED(result))
			{
				PrintC(Console::Error, "Failed to create sceneSRV");
				return false;
			}

			myDefferedNormalSRV->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Deffered Normal").length()), "Deffered Normal");
		}	

		// Creating Pixel position vectorImage
		{
			textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

			ID3D11Texture2D* texture = nullptr;
			hr = device->CreateTexture2D(&textureDesc, nullptr, &texture);

			if (FAILED(result))
			{
				PrintC(Console::Error, "Failed to retrieve backbuffer!");
				return false;
			}

			myDefferedPositionBuffer = std::make_shared<Texture>();
			result = device->CreateRenderTargetView(texture, nullptr, myDefferedPositionBuffer->myRTV.GetAddressOf());

			if (FAILED(result))
			{
				PrintC(Console::Error, "Failed to create render target view!");
				return false;
			}

			myDefferedPositionBuffer->myRTV->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Deffered Position").length()), "Deffered Position");

			result = device->CreateShaderResourceView(texture, nullptr, myDefferedPositionSRV.GetAddressOf());

			if (FAILED(result))
			{
				PrintC(Console::Error, "Failed to create sceneSRV");
				return false;
			}

			myDefferedPositionSRV->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Deffered Position").length()), "Deffered Position");
		}

		// Creating Material deffered
		{
			textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

			ID3D11Texture2D* texture = nullptr;
			hr = device->CreateTexture2D(&textureDesc, nullptr, &texture);

			if (FAILED(result))
			{
				PrintC(Console::Error, "Failed to retrieve backbuffer!");
				return false;
			}

			myDefferedMaterialBuffer = std::make_shared<Texture>();
			result = device->CreateRenderTargetView(texture, nullptr, myDefferedMaterialBuffer->myRTV.GetAddressOf());

			if (FAILED(result))
			{
				PrintC(Console::Error, "Failed to create render target view!");
				return false;
			}

			myDefferedMaterialBuffer->myRTV->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Deffered Material").length()), "Deffered Material");

			result = device->CreateShaderResourceView(texture, nullptr, myDefferedMaterialSRV.GetAddressOf());

			if (FAILED(result))
			{
				PrintC(Console::Error, "Failed to create sceneSRV");
				return false;
			}

			myDefferedMaterialSRV->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Deffered Material").length()), "Deffered Material");
		}

		// Creating pixel picker
		{
			D3D11_TEXTURE2D_DESC TextureDesciption = { };
			TextureDesciption.Width = 1;
			TextureDesciption.Height = 1;
			TextureDesciption.MipLevels = 1;
			TextureDesciption.ArraySize = 1;
			TextureDesciption.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			TextureDesciption.SampleDesc.Count = 1;
			TextureDesciption.Usage = D3D11_USAGE_STAGING;
			TextureDesciption.BindFlags = 0;
			TextureDesciption.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			TextureDesciption.MiscFlags = 0;

			hr = device->CreateTexture2D(&TextureDesciption, nullptr, &myGetPixelTexture);

			textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

			ID3D11Texture2D* texture = nullptr;
			hr = device->CreateTexture2D(&textureDesc, nullptr, &texture);

			if (FAILED(result))
			{
				PrintC(Console::Error, "Failed to retrieve backbuffer!");
				return false;
			}

			myDefferedPixelPickerBuffer = std::make_shared<Texture>();
			result = device->CreateRenderTargetView(texture, nullptr, myDefferedPixelPickerBuffer->myRTV.GetAddressOf());

			if (FAILED(result))
			{
				PrintC(Console::Error, "Failed to create render target view!");
				return false;
			}

			myDefferedPixelPickerBuffer->myRTV->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Deffered Pixel Picker").length()), "Deffered Pixel Picker");

			result = device->CreateShaderResourceView(texture, nullptr, myDefferedPixelPickerSRV.GetAddressOf());

			if (FAILED(result))
			{
				PrintC(Console::Error, "Failed to create sceneSRV");
				return false;
			}

			myDefferedPixelPickerSRV->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Deffered Pixel Picker").length()), "Deffered Pixel Picker");
		}
	}

	myBackBuffer->myViewPort = { 0, 0, clientWidth, clientHeight };

	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = static_cast<unsigned>(clientWidth);
	depthDesc.Height = static_cast<unsigned>(clientHeight);
	depthDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;

	result = device->CreateTexture2D(&depthDesc, nullptr, depthTexture.GetAddressOf());
	if (FAILED(result))
	{
		PrintC(Console::Error, "Failed to create depth buffer");
		return false;
	}

	result = device->CreateTexture2D(&depthDesc, nullptr, depthTextureScene.GetAddressOf());
	if (FAILED(result))
	{
		PrintC(Console::Error, "Failed to create depth buffer");
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};

	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	result = device->CreateDepthStencilView(depthTexture.Get(), &dsvDesc, myBackBuffer->myDSV.GetAddressOf());

	if (FAILED(result))
	{
		PrintC(Console::Error, "Failed to create depth stencil view");
		return false;
	}

	myBackBuffer->myDSV->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Back buffer").length()), "Back buffer");

	result = device->CreateDepthStencilView(depthTextureScene.Get(), &dsvDesc, mySceneBuffer->myDSV.GetAddressOf());

	if (FAILED(result))
	{
		PrintC(Console::Error, "Failed to create depth stencil view");
		return false;
	}

	mySceneBuffer->myDSV->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Scene buffer").length()), "Scene buffer");

	D3D11_SAMPLER_DESC samplerDesc = {};

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDesc, sSamplerState.GetAddressOf());
	if (FAILED(result)) 
	{
		PrintC(Console::Error, "Failed to create sampler state");
		return false;
	}

	sSamplerState->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Default").length()), "Default");

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

	result = device->CreateRasterizerState(&rasterizerDesc, rsPt.GetAddressOf());

	if (FAILED(result))
	{
		PrintC(Console::Error, "Failed to create rasterizer state");
		return false;
	}

	rsPt->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("Default").length()), "Default");

	rasterizerDesc.CullMode = D3D11_CULL_NONE;

	result = device->CreateRasterizerState(&rasterizerDesc, rsNoCulling.GetAddressOf());

	if (FAILED(result))
	{
		PrintC(Console::Error, "Failed to create rasterizer state");
		return false;
	}

	rsNoCulling->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::string("No culling").length()), "Default");

	myDevice = device;
	myContext = context;
	mySwapChain = swapChain;

	DisableD3D11Warning(myDevice.Get(), D3D11_MESSAGE_ID_DEVICE_DRAW_RENDERTARGETVIEW_NOT_SET);

	LOG(RhiLog, Log, "RHI initilize");

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGuiStyle& style = ImGui::GetStyle();

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplWin32_Init(aHWND);
	ImGui_ImplDX11_Init(this->myDevice.Get(), this->myContext.Get());

	return true;
}

CommonUtilities::Vector4<float> RenderHardwareInterface::GetPixelDX11(const int X, const int Y, ID3D11ShaderResourceView* SourceSRV)
{;

	// Get the underlying resource from the Shader Resource View (SRV)
	ID3D11Resource* SourceResource = nullptr;
	SourceSRV->GetResource(&SourceResource);

	RGBTRIPLE rgb;
	HRESULT hr = 0;
	D3D11_MAPPED_SUBRESOURCE MappedSubresource;
	D3D11_BOX srcBox;

	srcBox.left = X;
	srcBox.right = X + 1;
	srcBox.bottom = Y + 1;
	srcBox.top = Y;
	srcBox.front = 0;
	srcBox.back = 1;

	myContext->CopySubresourceRegion(myGetPixelTexture.Get(), 0, 0, 0, 0, SourceResource, 0, &srcBox);

	hr = myContext->Map(myGetPixelTexture.Get(), 0, D3D11_MAP_READ, 0, &MappedSubresource);

	uint32_t* pPixels = reinterpret_cast<uint32_t*>(MappedSubresource.pData);

	rgb.rgbtRed = (pPixels[0] >> 16) & 0xff;
	rgb.rgbtGreen = (pPixels[0] >> 8) & 0xff;
	rgb.rgbtBlue = pPixels[0] & 0xff;

	//std::cout << rgb.x << "    " << rgb.y << "    " << rgb.z << "    " << rgb.w << std::endl;

	return CommonUtilities::Vector4<float>();
}

void RenderHardwareInterface::Present() const
{
	mySwapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
}

bool RenderHardwareInterface::CreateIndexBuffer(const std::string_view aName, const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outIndexBuffer, bool aEditable)
{
	D3D11_BUFFER_DESC indexBufferDesc = {};

	indexBufferDesc.ByteWidth = static_cast<unsigned>(aIndexList.capacity() * sizeof(unsigned));
	indexBufferDesc.Usage = aEditable ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	if (aEditable)
	{
		indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	D3D11_SUBRESOURCE_DATA indexSubresourceData = {};

	indexSubresourceData.pSysMem = aIndexList.data();

	const HRESULT result = myDevice->CreateBuffer(&indexBufferDesc, &indexSubresourceData, outIndexBuffer.GetAddressOf());

	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create index buffer - {}", aName);
		return false;
	}

	//Set object name
	outIndexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(aName.length()), aName.data());

	return true;
}

bool RenderHardwareInterface::CreateConstantBuffer(const std::string_view aName, const size_t aSize, const unsigned aSlot, const unsigned aPipeLineStages, ConstantBuffer& outConstantBuffer)
{
	if (aSize > 65536)
	{
		LOG(RhiLog, Error, "Failed to create constant buffer {}, Size is larger than 64KB  size: {}", aName, aSize);
		return false;
	}

	outConstantBuffer.myName = aName;
	outConstantBuffer.mySize = aSize;
	outConstantBuffer.myPipeLineStages = aPipeLineStages;
	outConstantBuffer.mySlotIndex = aSlot;

	D3D11_BUFFER_DESC bufferDesc = {};

	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.ByteWidth = static_cast<unsigned>(aSize);

	const HRESULT result = myDevice->CreateBuffer(&bufferDesc, nullptr, outConstantBuffer.myBuffer.GetAddressOf());

	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create constant buffer for {}. Check DirectX log for more information", aName);
		return false;
	}

	//Set object name
	outConstantBuffer.myBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(aName.length()), aName.data());

	LOG(RhiLog, Log, "Succesfully created constant buffer for {}", aName);

	return true;
}



void RenderHardwareInterface::SetVertexBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aVertexBuffer, const size_t aVertexSize, const size_t aVertexOffset)
{
	const unsigned vertexSize = static_cast<const unsigned>(aVertexSize);
	const unsigned vertexOffset = static_cast<const unsigned>(aVertexOffset);

	myContext->IASetVertexBuffers(0, 1, aVertexBuffer.GetAddressOf(), &vertexSize, &vertexOffset);
}

void RenderHardwareInterface::SetVertexBuffers(const std::vector<ID3D11Buffer*>& aVertexBuffer, const std::vector < size_t>& aVertexSize, const std::vector < size_t>& aVertexOffset)
{
	std::vector <unsigned> vertexSize;
	std::vector <unsigned> vertexOffset;

	for (const auto& i : aVertexSize)
	{
		vertexSize.push_back(static_cast<unsigned>(i));
	}

	for (const auto& i : aVertexOffset)
	{
		vertexOffset.push_back(static_cast<unsigned>(i));
	}


	myContext->IASetVertexBuffers(0, static_cast<unsigned>(aVertexBuffer.size()), aVertexBuffer.data(), vertexSize.data(), vertexOffset.data());
}

void RenderHardwareInterface::SetVertexBuffers(const std::vector<ID3D11Buffer*>& aVertexBuffer, const std::vector < unsigned>& aVertexSize, const std::vector < unsigned>& aVertexOffset)
{
	myContext->IASetVertexBuffers(0, static_cast<unsigned>(aVertexBuffer.size()), aVertexBuffer.data(), aVertexSize.data(), aVertexOffset.data());
}

void RenderHardwareInterface::SetIndexBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aIndexBuffer) const
{
	const DXGI_FORMAT indexBufferFormat = DXGI_FORMAT_R32_UINT;

	myContext->IASetIndexBuffer(aIndexBuffer.Get(), indexBufferFormat, 0);
}

void RenderHardwareInterface::SetConstantBuffer(const ConstantBuffer& aBuffer)
{
	if (aBuffer.myPipeLineStages & PIPELINE_STAGE_PIXEL_SHADER)
	{
		myContext->PSSetConstantBuffers(aBuffer.mySlotIndex, 1, aBuffer.myBuffer.GetAddressOf());
	}
	if (aBuffer.myPipeLineStages & PIPELINE_STAGE_VERTEX_SHADER)
	{
		myContext->VSSetConstantBuffers(aBuffer.mySlotIndex, 1, aBuffer.myBuffer.GetAddressOf());
	}
	if (aBuffer.myPipeLineStages & PIPELINE_STAGE_GEOMETRY_SHADER)
	{
		myContext->GSSetConstantBuffers(aBuffer.mySlotIndex, 1, aBuffer.myBuffer.GetAddressOf());
	}
}

void RenderHardwareInterface::SetPrimitiveTopology(unsigned aTopology) const
{
	myContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(aTopology));
}

bool RenderHardwareInterface::CreateInputLayout(
	Microsoft::WRL::ComPtr<ID3D11InputLayout>& outInputLayout,
	const std::vector<VertexElementDesc>& aInputLayoutDefinition,
	const void* vsBytecode,
	const size_t vsBytecodeSize)
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
	inputElements.reserve(aInputLayoutDefinition.size());

	for (const auto& vxED : aInputLayoutDefinition)
	{
		D3D11_INPUT_ELEMENT_DESC element = {};
		element.SemanticName = vxED.Semantic.data();
		element.SemanticIndex = vxED.SemanticIndex;
		element.Format = static_cast<DXGI_FORMAT>(vxED.Type);

		element.InputSlot = static_cast<int>(vxED.IsInstancedData);
		element.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		element.InputSlotClass = vxED.IsInstancedData ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
		element.InstanceDataStepRate = static_cast<int>(vxED.IsInstancedData);

		inputElements.emplace_back(element);
	}

	// Create the input layout using the provided shader bytecode.
	HRESULT result = myDevice->CreateInputLayout(
		inputElements.data(),
		static_cast<UINT>(inputElements.size()),
		vsBytecode,
		vsBytecodeSize,
		outInputLayout.GetAddressOf()
	);

	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create input layout!");
		return false;
	}

	std::string name = "Input layout";

	outInputLayout->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.length()), name.c_str());

	return true;
}

void RenderHardwareInterface::SetInputLayout(Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout)
{
	myContext->IASetInputLayout(aInputLayout.Get());
}

// depricated
bool RenderHardwareInterface::LoadShaderFromMemory(const std::string_view aName, Shader& outShader, const uint8_t* aShaderDataPtr, const size_t aShaderDataSize)
{
	aName;
	ComPtr<ID3D11ShaderReflection> shaderReflection;

	HRESULT result = D3DReflect(
		aShaderDataPtr,
		aShaderDataSize,
		IID_ID3D11ShaderReflection,
		reinterpret_cast<void**>(shaderReflection.GetAddressOf())
	);

	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to load Shader!");
	}

	D3D11_SHADER_DESC shaderDesc = {};
	shaderReflection->GetDesc(&shaderDesc);

	D3D11_SHADER_VERSION_TYPE shaderVersion = static_cast<D3D11_SHADER_VERSION_TYPE>(D3D11_SHVER_GET_TYPE(shaderDesc.Version));

	switch (shaderVersion)
	{
	case D3D11_SHVER_VERTEX_SHADER:
	{
		outShader.myType = ShaderType::VertexShader;

		ComPtr<ID3D11VertexShader> vxShader;
		result = myDevice->CreateVertexShader(aShaderDataPtr, aShaderDataSize, nullptr, &vxShader);
		outShader.myshader = vxShader;

		break;
	}

	case D3D11_SHVER_PIXEL_SHADER:
	{
		outShader.myType = ShaderType::PixelShader;

		ComPtr<ID3D11PixelShader> pxShader;
		result = myDevice->CreatePixelShader(aShaderDataPtr, aShaderDataSize, nullptr, &pxShader);
		outShader.myshader = pxShader;

		break;
	}

	case D3D11_SHVER_GEOMETRY_SHADER:
	{
		outShader.myType = ShaderType::GeometryShader;

		ComPtr<ID3D11GeometryShader> gsShader;
		result = myDevice->CreateGeometryShader(aShaderDataPtr, aShaderDataSize, nullptr, &gsShader);
		outShader.myshader = gsShader;

		break;
	}

	}

	//outShader.myBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(aName.length()), aName.data());

	return true;
}

bool RenderHardwareInterface::LoadPSFromFile(std::wstring shaderName, Shader& outShader, const std::wstring& shaderPath)
{
	outShader.myType = ShaderType::PixelShader;

	ComPtr<ID3D11PixelShader> pxShader;

	HRESULT result;

	std::ifstream psFile;
	psFile.open(shaderPath, std::ios::binary);
	std::string data = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
	result = myDevice->CreatePixelShader(data.data(), data.size(), nullptr, pxShader.ReleaseAndGetAddressOf());

	outShader.myshader = pxShader;

	outShader.myName = shaderName;
	outShader.myPath = shaderPath;

	if (FAILED(result))
	{
		return false;
	}

	psFile.close();

	return true;
}

bool RenderHardwareInterface::LoadVSFromFile(std::wstring shaderName, Shader& outShader, const std::wstring& shaderPath, std::string& data)
{
	outShader.myType = ShaderType::VertexShader;

	ComPtr<ID3D11VertexShader> vxShader;

	HRESULT result;

	std::ifstream vsFile;
	vsFile.open(shaderPath.c_str(), std::ios::binary);
	data = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	result = myDevice->CreateVertexShader(data.data(), data.size(), nullptr, vxShader.ReleaseAndGetAddressOf());

	outShader.myshader = vxShader;

	outShader.myName = shaderName;
	outShader.myPath = shaderPath;

	if (FAILED(result))
	{
		return false;
	}
	vsFile.close();

	return true;
}


void RenderHardwareInterface::Draw(const unsigned aVertexCount)
{
	assert(!(aVertexCount % 3) && "A shape does not have verticies divicible by 3 (does not form triangles)");

	myContext->Draw(aVertexCount, 0);
}

void RenderHardwareInterface::DrawIndexed(unsigned aStartIndex, unsigned aIndexCount, unsigned aVertexOffset, Material& aMaterial) const
{
	myContext->PSSetSamplers(0, 1, sSamplerState.GetAddressOf());

	DefaultConstants defaultConstants;

	defaultConstants.useTexture = 0;

	if (aMaterial.GetPSShader()->GetDiffuseShaderResourceView())
	{
		defaultConstants.useTexture |= static_cast<int>(UseTexture::Diffuse);
		myContext->PSSetShaderResources(0, 1, aMaterial.GetPSShader()->GetDiffuseShaderResourceView().GetAddressOf());
	}
	else
	{
		ID3D11ShaderResourceView* nullSRV = nullptr;
		myContext->PSSetShaderResources(0, 1, &nullSRV);
	}

	if (aMaterial.GetPSShader()->GetNormalShaderResourceView())
	{
		defaultConstants.useTexture |= static_cast<int>(UseTexture::Normal);
		myContext->PSSetShaderResources(1, 1, aMaterial.GetPSShader()->GetNormalShaderResourceView().GetAddressOf());
	}
	else
	{
		ID3D11ShaderResourceView* nullSRV = nullptr;
		myContext->PSSetShaderResources(1, 1, &nullSRV);
	}

	if (aMaterial.GetPSShader()->GetRoughnessShaderResourceView())
	{
		defaultConstants.useTexture |= static_cast<int>(UseTexture::Roughness);
		myContext->PSSetShaderResources(2, 1, aMaterial.GetPSShader()->GetRoughnessShaderResourceView().GetAddressOf());
	}
	else
	{
		ID3D11ShaderResourceView* nullSRV = nullptr;
		myContext->PSSetShaderResources(2, 1, &nullSRV);
	}

	if (aMaterial.GetPSShader()->GetMetallicShaderResourceView())
	{
		defaultConstants.useTexture |= static_cast<int>(UseTexture::Metallic);
		myContext->PSSetShaderResources(3, 1, aMaterial.GetPSShader()->GetMetallicShaderResourceView().GetAddressOf());
	}
	else
	{
		ID3D11ShaderResourceView* nullSRV = nullptr;
		myContext->PSSetShaderResources(3, 1, &nullSRV);
	}

	if (aMaterial.GetPSShader()->GetAOShaderResourceView())
	{
		defaultConstants.useTexture |= static_cast<int>(UseTexture::AO);
		myContext->PSSetShaderResources(4, 1, aMaterial.GetPSShader()->GetAOShaderResourceView().GetAddressOf());
	}
	else
	{
		ID3D11ShaderResourceView* nullSRV = nullptr;
		myContext->PSSetShaderResources(4, 1, &nullSRV);
	}

	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::Defaultconstants, defaultConstants);

	myContext->DrawIndexed(aIndexCount, aStartIndex, aVertexOffset);
}

void RenderHardwareInterface::DrawInstanced(unsigned aIndexCount, const unsigned aIndexOffset, const unsigned aVertexOffset, unsigned aInstanceCount, Material& aMaterial) const
{
	myContext->PSSetSamplers(0, 1, sSamplerState.GetAddressOf());

	DefaultConstants defaultConstants;

	defaultConstants.useTexture = 0;

	if (aMaterial.GetPSShader()->GetDiffuseShaderResourceView())
	{
		defaultConstants.useTexture |= static_cast<int>(UseTexture::Diffuse);
		myContext->PSSetShaderResources(0, 1, aMaterial.GetPSShader()->GetDiffuseShaderResourceView().GetAddressOf());
	}
	else
	{
		ID3D11ShaderResourceView* nullSRV = nullptr;
		myContext->PSSetShaderResources(0, 1, &nullSRV);
	}

	if (aMaterial.GetPSShader()->GetNormalShaderResourceView())
	{
		defaultConstants.useTexture |= static_cast<int>(UseTexture::Normal);
		myContext->PSSetShaderResources(1, 1, aMaterial.GetPSShader()->GetNormalShaderResourceView().GetAddressOf());
	}
	else
	{
		ID3D11ShaderResourceView* nullSRV = nullptr;
		myContext->PSSetShaderResources(1, 1, &nullSRV);
	}

	if (aMaterial.GetPSShader()->GetRoughnessShaderResourceView())
	{
		defaultConstants.useTexture |= static_cast<int>(UseTexture::Roughness);
		myContext->PSSetShaderResources(2, 1, aMaterial.GetPSShader()->GetRoughnessShaderResourceView().GetAddressOf());
	}
	else
	{
		ID3D11ShaderResourceView* nullSRV = nullptr;
		myContext->PSSetShaderResources(2, 1, &nullSRV);
	}

	if (aMaterial.GetPSShader()->GetMetallicShaderResourceView())
	{
		defaultConstants.useTexture |= static_cast<int>(UseTexture::Metallic);
		myContext->PSSetShaderResources(3, 1, aMaterial.GetPSShader()->GetMetallicShaderResourceView().GetAddressOf());
	}
	else
	{
		ID3D11ShaderResourceView* nullSRV = nullptr;
		myContext->PSSetShaderResources(3, 1, &nullSRV);
	}

	if (aMaterial.GetPSShader()->GetAOShaderResourceView())
	{
		defaultConstants.useTexture |= static_cast<int>(UseTexture::AO);
		myContext->PSSetShaderResources(4, 1, aMaterial.GetPSShader()->GetAOShaderResourceView().GetAddressOf());
	}
	else
	{
		ID3D11ShaderResourceView* nullSRV = nullptr;
		myContext->PSSetShaderResources(4, 1, &nullSRV);
	}

	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::Defaultconstants, defaultConstants);

	myContext->DrawIndexedInstanced(aIndexCount, aInstanceCount, aIndexOffset, aVertexOffset, 0);
}

void RenderHardwareInterface::DrawTextured(const unsigned aVertexCount, Material& aMaterial) const
{
	myContext->PSSetSamplers(0, 1, sSamplerState.GetAddressOf());

	DefaultConstants defaultConstants;

	defaultConstants.useTexture = 0;

	if (aMaterial.GetPSShader()->GetDiffuseShaderResourceView())
	{
		defaultConstants.useTexture |= static_cast<int>(UseTexture::Diffuse);
		myContext->PSSetShaderResources(0, 1, aMaterial.GetPSShader()->GetDiffuseShaderResourceView().GetAddressOf());
	}
	else
	{
		ID3D11ShaderResourceView* nullSRV = nullptr;
		myContext->PSSetShaderResources(0, 1, &nullSRV);
	}

	if (aMaterial.GetPSShader()->GetNormalShaderResourceView())
	{
		defaultConstants.useTexture |= static_cast<int>(UseTexture::Normal);
		myContext->PSSetShaderResources(1, 1, aMaterial.GetPSShader()->GetNormalShaderResourceView().GetAddressOf());
	}
	else
	{
		ID3D11ShaderResourceView* nullSRV = nullptr;
		myContext->PSSetShaderResources(1, 1, &nullSRV);
	}

	if (aMaterial.GetPSShader()->GetRoughnessShaderResourceView())
	{
		defaultConstants.useTexture |= static_cast<int>(UseTexture::Roughness);
		myContext->PSSetShaderResources(2, 1, aMaterial.GetPSShader()->GetRoughnessShaderResourceView().GetAddressOf());
	}
	else
	{
		ID3D11ShaderResourceView* nullSRV = nullptr;
		myContext->PSSetShaderResources(2, 1, &nullSRV);
	}

	if (aMaterial.GetPSShader()->GetMetallicShaderResourceView())
	{
		defaultConstants.useTexture |= static_cast<int>(UseTexture::Metallic);
		myContext->PSSetShaderResources(3, 1, aMaterial.GetPSShader()->GetMetallicShaderResourceView().GetAddressOf());
	}
	else
	{
		ID3D11ShaderResourceView* nullSRV = nullptr;
		myContext->PSSetShaderResources(3, 1, &nullSRV);
	}

	if (aMaterial.GetPSShader()->GetAOShaderResourceView())
	{
		defaultConstants.useTexture |= static_cast<int>(UseTexture::AO);
		myContext->PSSetShaderResources(4, 1, aMaterial.GetPSShader()->GetAOShaderResourceView().GetAddressOf());
	}
	else
	{
		ID3D11ShaderResourceView* nullSRV = nullptr;
		myContext->PSSetShaderResources(4, 1, &nullSRV);
	}

	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::Defaultconstants, defaultConstants);

	myContext->Draw(aVertexCount, 0);
}

void RenderHardwareInterface::DrawIndexedTexture(const unsigned aStartIndex, const unsigned aIndexCount, const unsigned aVertexOffset, const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aTextrure) const
{
	myContext->PSSetSamplers(0, 1, sSamplerState.GetAddressOf());

	myContext->PSSetShaderResources(0, 1, aTextrure.GetAddressOf());

	myContext->DrawIndexed(aIndexCount, aStartIndex, aVertexOffset);
}

void RenderHardwareInterface::DrawVertexTexture(const unsigned aVertexCount, const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aTextrure) const
{
	myContext->PSSetSamplers(0, 1, sSamplerState.GetAddressOf());

	myContext->PSSetShaderResources(0, 1, aTextrure.GetAddressOf());

	myContext->Draw(aVertexCount, 0);
}

void RenderHardwareInterface::DrawIndexedSprite(const unsigned aStartIndex, const unsigned aIndexCount, const unsigned aVertexOffset, Sprite* aSprite) const
{
	myContext->PSSetSamplers(0, 1, sSamplerState.GetAddressOf());

	DefaultConstants defaultConstants;

	defaultConstants.useTexture = 0;

	if (aSprite->aTexture)
	{
		defaultConstants.useTexture |= static_cast<int>(UseTexture::Diffuse);
		myContext->PSSetShaderResources(0, 1, aSprite->aTexture.GetAddressOf());
	}
	else
	{
		ID3D11ShaderResourceView* nullSRV = nullptr;
		myContext->PSSetShaderResources(0, 1, &nullSRV);
	}

	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::Defaultconstants, defaultConstants);

	myContext->DrawIndexed(aIndexCount, aStartIndex, aVertexOffset);
}

void RenderHardwareInterface::ChangePipelineState(const PipelineStateObject& aNewPSO, const PipelineStateObject& aOldPSO)
{
	if (aOldPSO.RenderTarget)
	{
		ID3D11RenderTargetView* nullRtv = nullptr;
		myContext->OMSetRenderTargets(1, &nullRtv, nullptr);
	}
	else if (aOldPSO.DepthStencil)
	{
		myContext->OMSetRenderTargets(0, nullptr, nullptr);
	}

	const std::array<float, 4> blendfactor = { 0, 0, 0, 0 };
	constexpr unsigned samplerMask = 0xffffffff;

	myContext->OMSetBlendState(aNewPSO.BlendState.Get(), blendfactor.data(), samplerMask);
	myContext->RSSetState(aNewPSO.RasterizerState.Get());
	myContext->OMSetDepthStencilState(aNewPSO.DepthStencilState.Get(), 0);

	myContext->IASetInputLayout(aNewPSO.InputLayout.Get());

	if (aNewPSO.ClearRenderTarget)
	{
		myContext->ClearRenderTargetView(aNewPSO.RenderTarget->myRTV.Get(), aNewPSO.RenderTarget->myClearColor.data());
	}
	if (aNewPSO.ClearDepthStencil)
	{
		myContext->ClearDepthStencilView(aNewPSO.DepthStencil->myDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	}

	myContext->OMSetRenderTargets(1, aNewPSO.RenderTarget->myRTV.GetAddressOf(), aNewPSO.DepthStencil->myDSV.Get());
}

Microsoft::WRL::ComPtr<ID3D11Device>& RenderHardwareInterface::GetDevice()
{
	return myDevice;
}

Microsoft::WRL::ComPtr<ID3D11DeviceContext>& RenderHardwareInterface::GetContext()
{
	return myContext;
}

void RenderHardwareInterface::ClearBackBuffer() const
{
	myContext->ClearRenderTargetView(myBackBuffer->myRTV.Get(), myBackBuffer->myClearColor.data());
	myContext->ClearDepthStencilView(myBackBuffer->myDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void RenderHardwareInterface::ClearSceneBuffer() const
{
	myContext->ClearRenderTargetView(mySceneBuffer->myRTV.Get(), mySceneBuffer->myClearColor.data());
	myContext->ClearDepthStencilView(mySceneBuffer->myDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void RenderHardwareInterface::ClearPPBuffer() const
{
	myContext->ClearRenderTargetView(myPostProcessBuffer->myRTV.Get(), myBackBuffer->myClearColor.data());
}

void RenderHardwareInterface::ClearDefferedBuffer() const
{
	std::array<float, 4> clearColor = { 0, 0, 0, 0 };

	myContext->ClearRenderTargetView(myDefferedAlbedoBuffer->myRTV.Get(), clearColor.data());
	myContext->ClearRenderTargetView(myDefferedNormalBuffer->myRTV.Get(), clearColor.data());
	myContext->ClearRenderTargetView(myDefferedPositionBuffer->myRTV.Get(), clearColor.data());
	myContext->ClearRenderTargetView(myDefferedMaterialBuffer->myRTV.Get(), clearColor.data());
	myContext->ClearRenderTargetView(myDefferedPixelPickerBuffer->myRTV.Get(), clearColor.data());
}

void RenderHardwareInterface::SetGeometryShader(const Shader& aGxShader)
{
	ComPtr<ID3D11GeometryShader> gxShader;

	aGxShader.myshader.As(&gxShader);
	myContext->GSSetShader(gxShader.Get(), nullptr, 0);
}

void RenderHardwareInterface::ResetGeometryShader()
{
	myContext->GSSetShader(nullptr, nullptr, 0);
}

void RenderHardwareInterface::SetShaders(const Shader& aVxShader, const Shader& aPxShader)
{
	ComPtr<ID3D11VertexShader> vxShader;

	aVxShader.myshader.As(&vxShader);
	myContext->VSSetShader(vxShader.Get(), nullptr, 0);

	ComPtr<ID3D11PixelShader> pxShader;

	aPxShader.myshader.As(&pxShader);
	myContext->PSSetShader(pxShader.Get(), nullptr, 0);
}

void RenderHardwareInterface::SetShaders(const Shader& aVxShader, const Microsoft::WRL::ComPtr<ID3D11PixelShader> pxShader)
{
	ComPtr<ID3D11VertexShader> vxShader;

	aVxShader.myshader.As(&vxShader);
	myContext->VSSetShader(vxShader.Get(), nullptr, 0);

	myContext->PSSetShader(pxShader.Get(), nullptr, 0);
}

void RenderHardwareInterface::ClearRenderTargetView(Microsoft::WRL::ComPtr<ID3D11RenderTargetView> aRTV) const
{
	std::array<float, 4> clearColor = { 0, 0, 0, 0 };

	myContext->ClearRenderTargetView(aRTV.Get(), clearColor.data());
}

void RenderHardwareInterface::SetRenderTargetView(Microsoft::WRL::ComPtr<ID3D11RenderTargetView> aRTV)
{
	myContext->OMSetRenderTargets(1, aRTV.GetAddressOf(), nullptr);

	D3D11_VIEWPORT viewPort = {};

	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = myBackBuffer->myViewPort.z;
	viewPort.Height = myBackBuffer->myViewPort.w;

	viewPort.MinDepth = 0;
	viewPort.MaxDepth = 1;

	myContext->RSSetViewports(1, &viewPort);
}

void RenderHardwareInterface::SetRenderTargetView(Microsoft::WRL::ComPtr<ID3D11RenderTargetView> aRTV, D3D11_VIEWPORT aViewPort)
{
	myContext->OMSetRenderTargets(1, aRTV.GetAddressOf(), nullptr);

	myContext->RSSetViewports(1, &aViewPort);
}

void RenderHardwareInterface::SetBackBufferRT()
{

	myContext->OMSetRenderTargets(1, myBackBuffer->myRTV.GetAddressOf(), myBackBuffer->myDSV.Get());

	D3D11_VIEWPORT viewPort = {};

	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = myBackBuffer->myViewPort.z;
	viewPort.Height = myBackBuffer->myViewPort.w;

	viewPort.MinDepth = 0;
	viewPort.MaxDepth = 1;

	myContext->RSSetViewports(1, &viewPort);
}

void RenderHardwareInterface::SetSceneBufferRT(const bool depthBuffer)
{
	myContext->OMSetRenderTargets(1, mySceneBuffer->myRTV.GetAddressOf(), depthBuffer ? mySceneBuffer->myDSV.Get() : nullptr);

	D3D11_VIEWPORT viewPort = {};

	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = myBackBuffer->myViewPort.z;
	viewPort.Height = myBackBuffer->myViewPort.w;

	viewPort.MinDepth = 0;
	viewPort.MaxDepth = 1;

	myContext->RSSetViewports(1, &viewPort);
}

void RenderHardwareInterface::SetDefferedBufferRT()
{
	std::array<ID3D11RenderTargetView*, 5> rtvs = { 
		myDefferedAlbedoBuffer->myRTV.Get(), 
		myDefferedNormalBuffer->myRTV.Get(), 
		myDefferedPositionBuffer->myRTV.Get(), 
		myDefferedMaterialBuffer->myRTV.Get(), 
		myDefferedPixelPickerBuffer->myRTV.Get() };

	myContext->OMSetRenderTargets(5, rtvs.data(), mySceneBuffer->myDSV.Get());

	D3D11_VIEWPORT viewPort = {};

	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = myBackBuffer->myViewPort.z;
	viewPort.Height = myBackBuffer->myViewPort.w;

	viewPort.MinDepth = 0;
	viewPort.MaxDepth = 1;

	myContext->RSSetViewports(1, &viewPort);

	MainSingleton::Get()->shaderResources->SetShaderObjects();
}

void RenderHardwareInterface::SetPostProcessBufferRT(const bool useDepth)
{
	myContext->OMSetRenderTargets(1, myPostProcessBuffer->myRTV.GetAddressOf(), useDepth ? mySceneBuffer->myDSV.Get() : nullptr);

	D3D11_VIEWPORT viewPort = {};

	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = myBackBuffer->myViewPort.z;
	viewPort.Height = myBackBuffer->myViewPort.w;

	viewPort.MinDepth = 0;
	viewPort.MaxDepth = 1;

	myContext->RSSetViewports(1, &viewPort);
}

bool RenderHardwareInterface::CreateVertexBufferInternal(const std::string_view aName, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer, const uint8_t* aVertexDataPointer, const size_t aNumVerticies, const size_t aVertexSize, bool aEditable)
{
	D3D11_BUFFER_DESC vxBufferDesc = {};
	vxBufferDesc.ByteWidth = static_cast<unsigned>(aNumVerticies * aVertexSize);
	vxBufferDesc.Usage = aEditable ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
	vxBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	if (aEditable)
	{
		vxBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	D3D11_SUBRESOURCE_DATA vxResource = {};
	vxResource.pSysMem = aVertexDataPointer;

	const HRESULT result = myDevice->CreateBuffer(&vxBufferDesc, &vxResource, outVxBuffer.GetAddressOf());

	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create Vertex Buffer!");
		return false;
	}

	outVxBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(aName.length()), aName.data());

	return true;
}

template <class VertexType>
bool RenderHardwareInterface::UpdateVertexBuffer(const std::vector<VertexType>& aVertexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer)
{
	const size_t vxSize = sizeof(VertexType);
	const size_t vxCount = aVertexList.size();

	return UpdateVertexBufferInternal(outVxBuffer, reinterpret_cast<const uint8_t*>(aVertexList.data()), vxCount * vxSize);
}

bool RenderHardwareInterface::UpdateIndexBuffer(const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& aVxBuffer)
{
	if (aIndexList.empty())
		return false;

	const size_t vxCountSource = aIndexList.size();

	return UpdateIndexBufferInternal(aVxBuffer, aIndexList.data(), vxCountSource);
}

bool RenderHardwareInterface::UpdateVertexBufferInternal(Microsoft::WRL::ComPtr<ID3D11Buffer>& aVxBuffer, const void* aBufferData, const size_t aSourceSize)
{
	if (!aVxBuffer || !aBufferData)
	{
		//LOG(RhiLog, Error, "Invalid arguments passed to UpdateVertexBufferInternal.");
		return false;
	}

	D3D11_MAPPED_SUBRESOURCE bufferdata = {};
	HRESULT result = myContext->Map(aVxBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferdata);
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to map vertex buffer. HRESULT: %08X", result);
		return false;
	}

	memcpy_s(bufferdata.pData, aSourceSize, aBufferData, aSourceSize);
	myContext->Unmap(aVxBuffer.Get(), 0);

	return true;
}


bool RenderHardwareInterface::UpdateIndexBufferInternal(Microsoft::WRL::ComPtr<ID3D11Buffer>& aIdxBuffer, const void* aBufferData, const size_t aNumIndices)
{
	if (!aIdxBuffer || !aBufferData)
	{
		//LOG(RhiLog, Error, "Invalid arguments passed to UpdateIndexBufferInternal.");
		return false;
	}

	unsigned sizeSource = static_cast<unsigned>(aNumIndices * sizeof(unsigned));

	D3D11_MAPPED_SUBRESOURCE bufferdata = {};
	HRESULT result = myContext->Map(aIdxBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferdata);

	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to map index buffer. HRESULT: %08X", result);
		return false;
	}

	memcpy_s(bufferdata.pData, sizeSource, aBufferData, sizeSource);
	myContext->Unmap(aIdxBuffer.Get(), 0);
	return true;
}

bool RenderHardwareInterface::UpdateConstantBufferInternal(const ConstantBuffer& aBuffer, const void* aBufferData, const size_t aBufferdataSize)
{
	if (!aBuffer.myBuffer)
	{
		LOG(RhiLog, Error, "Failed to update constant buffer. Bufferr {} is invalid", aBuffer.myName);
		return false;
	}

	if (aBufferdataSize > aBuffer.mySize)
	{
		LOG(RhiLog, Error, "Failed to update constant buffer. Data to large for buffer, size: {}", aBufferdataSize);
		return false;
	}

	D3D11_MAPPED_SUBRESOURCE bufferdata = {};

	HRESULT result = myContext->Map(aBuffer.myBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferdata);

	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to map constant buffer {}.", aBuffer.myName);
	}

	memcpy_s(bufferdata.pData, aBuffer.mySize, aBufferData, aBufferdataSize);

	myContext->Unmap(aBuffer.myBuffer.Get(), 0);

	return true;
}
