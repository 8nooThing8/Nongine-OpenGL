#pragma once

#include <memory> 

#include <wrl.h>

#include "PostProcessing.h"
#include <atomic>

#include "ModelSprite/ModelFactory.h"

class GameObject;
class SpriteRenderer;

struct ID3D11BlendState;
struct ID3D11SamplerState;

class DefferedRendering
{
public:
	DefferedRendering() = default;
	~DefferedRendering() = default;

	void Render();
	void Init(HWND aHWND);

	std::atomic_bool myFinishedRendering = true;
private:
	PostProcessing myPostProcessing;

	std::shared_ptr<GameObject> aFullScreensprite;
	std::shared_ptr<GameObject> aZero;

	SpriteRenderer* aFullScreenspriteRenderer;
	SpriteRenderer* aPostprocessSprite;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> myLutSampler;

	Microsoft::WRL::ComPtr<ID3D11BlendState> myBlendstate;


};

