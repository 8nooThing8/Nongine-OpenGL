#pragma once

#include <memory> 

#include <vector>
#include <wrl.h>

class SpriteRenderer;
class GameObject;

struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11SamplerState;
struct D3D11_TEXTURE2D_DESC;

enum PPType : unsigned
{
	NONE,
	SSAOPP,
	Bloom,
	DownSample,
	GaussianBlurHor,
	GaussianBlurVer,
	UpSample,
	ToneMap,
	PPEffectCount
};

struct PostProcessResource
{
	std::shared_ptr<D3D11_TEXTURE2D_DESC> textureDesc;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
	SpriteRenderer* spriteRenderer;
	PPType type;

	std::string typeToString()
	{
		switch (type)
		{
		case NONE:
			return "None";
		case SSAOPP:
			return "SSAO";
		case Bloom:
			return "Bloom";
		case DownSample:
			return "DownSample";
		case GaussianBlurHor:
			return "GaussianBlurHor";
		case GaussianBlurVer:
			return "GaussianBlurVer";
		case UpSample:
			return "UpSample";
		case ToneMap:
			return "ToneMap";
		case PPEffectCount:
			return "PPEffectCount";
		}

		return "";
	}

	PostProcessResource(PPType aPPType, std::shared_ptr <D3D11_TEXTURE2D_DESC> aTD);
};

class PostProcessing
{
public:

	friend class DefferedRendering;

	PostProcessing() = default;
	~PostProcessing() = default;

	ID3D11ShaderResourceView* Render(ID3D11ShaderResourceView* inSRV);
	void Init(HWND aHWND);
private:
	std::shared_ptr<GameObject> mySprite;
	std::vector<PostProcessResource*> textures;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> myPostProcessSampler;

	int postProcessUsing;

	int ppCount;

	int tonemap = 0;

	int GetPPEffectsCount();
	int GetNextPPEffect(int aNext);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myBlueNoise;
};

