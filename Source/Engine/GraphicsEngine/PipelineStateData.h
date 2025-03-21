#pragma once

#include <wrl.h>
#include "Texture.h"

struct ID3D11BlendState;
struct ID3D11RasterizerState;
struct ID3D11DepthStencilState;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11InputLayout;

struct PipelineStateObject
{
	PipelineStateObject();
	~PipelineStateObject();

	Microsoft::WRL::ComPtr<ID3D11BlendState> BlendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> RasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout;

	unsigned VertexStride = 0;

	std::shared_ptr<Texture> RenderTarget;
	bool ClearRenderTarget = false;

	std::shared_ptr<Texture> DepthStencil;
	bool ClearDepthStencil = false;
};
