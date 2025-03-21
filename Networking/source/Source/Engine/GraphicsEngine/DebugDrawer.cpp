#include "../Engine.pch.h"

//#include "DebugDrawer.h"
//#include "RenderHardwareInterface.h"
//#include "Graphics\GraphicsEngine\RenderHardwareInterface.h"
//
//namespace Debug
//{
//	void DrawSphere()
//	{
//		if (!myWireFrame)
//		{
//			Init();
//		}
//
//		RenderHardwareInterface::myContext->RSSetState(myWireFrame.Get());
//
//		RenderHardwareInterface::Draw();
//	}
//
//	void Init()
//	{
//		D3D11_RASTERIZER_DESC rasterDesc;
//		ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
//
//		rasterDesc.FillMode = D3D11_FILL_SOLID;
//		rasterDesc.CullMode = D3D11_CULL_FRONT;
//		rasterDesc.FrontCounterClockwise = false;
//		rasterDesc.DepthBias = 0;
//		rasterDesc.DepthBiasClamp = 0.0f;
//		rasterDesc.SlopeScaledDepthBias = 0.0f;
//		rasterDesc.DepthClipEnable = true;
//		rasterDesc.ScissorEnable = false;
//		rasterDesc.MultisampleEnable = false;
//		rasterDesc.AntialiasedLineEnable = false;
//
//		HRESULT result = RenderHardwareInterface::myDevice->CreateRasterizerState(&rasterDesc, myWireFrame.GetAddressOf());
//		if (FAILED(result))
//		{
//			
//			return;
//		}
//	}
//}