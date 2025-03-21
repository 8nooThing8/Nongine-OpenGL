#pragma once

#include <memory> 

#include <vector>

#include <wrl.h>

#include "PostProcessing.h"
#include <atomic>

#include "ModelSprite/ModelFactory.h"

#include "utility"
#include "functional"

#include <set>

class GameObject;
class SpriteRenderer;
class MeshRenderer;

struct ID3D11BlendState;
struct ID3D11SamplerState;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilState;
struct ID3D11ShaderResourceView;
struct ID3D11RasterizerState;

class DefferedRendering
{
public:
	static DefferedRendering& Get()
	{
		static DefferedRendering instance;
		return instance;
	}

	DefferedRendering() = default;
	~DefferedRendering() = default;

	void Render();
	void Init(HWND aHWND);

	void RenderAllMeshes();

	std::atomic_bool myFinishedRendering = true;
	static inline std::set<int> layers;
private:
	ID3D11RasterizerState* myHoleState;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> holeStencilState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	D3D11_VIEWPORT myViewPort;

	PostProcessing myPostProcessing;

	std::shared_ptr<GameObject> aFullScreensprite;
	std::shared_ptr<GameObject> aZero;

	SpriteRenderer* aFullScreenspriteRenderer;
	SpriteRenderer* aPostprocessSprite;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> myLutSampler;

	Microsoft::WRL::ComPtr<ID3D11BlendState> myNoBlendstate;
	Microsoft::WRL::ComPtr<ID3D11BlendState> myBlendstate;

	static inline int maxLayer = 0;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rtvSRV;

	std::vector<std::pair<float, std::function<void()>>> myLayerMeshRenderer;

	//Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv;
	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> dsvSRV;
};

