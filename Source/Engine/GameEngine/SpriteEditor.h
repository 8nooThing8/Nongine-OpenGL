#pragma once

#include <wrl.h>
#include <memory>

#include "ImGui/imgui.h"

#include <filesystem>

#include "../Engine/GraphicsEngine/Buffers/SpriteEditorBuffer.h"

class SpriteRenderer;

struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct ID3D11SamplerState;
class GameObject;

class SpriteEditor
{
public:
	SpriteEditor();
	void SaveSpriteConfig();
	~SpriteEditor() = default;

	void Initilize(ID3D11ShaderResourceView* aImage, std::filesystem::path aPath);

	void Update();

private:
	bool FindMetaFile();

	enum TouchingEdge : int
	{
		Left = 1,
		Right = 2,
		Up = 4,
		Down = 8
	};

	void CloseSpriteEditor();

	int isOnSelectedBorder(CommonUtilities::Vector2<float> aPos);
	void SetCursorInternal(int input);
	void PropertiesWindow(const ImVec2& windowPosition, const ImVec2& windowSize);

	std::filesystem::path myImagePath;

	ImVec2 myImageSize;
	ID3D11ShaderResourceView* myImageSRV;

	ID3D11ShaderResourceView* myImageWithBackgroundSRV;
	ID3D11RenderTargetView* myImageWithBackgroundRTV;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> myPointSampler;

	float mySizeMultiplier = 1.f;
	float myPrevSizeMultiplier = 1.f;

	float totalZoom = 1;

	float scrollSpeed = 0.001f;

	float myScrollDelta;

	ImVec2 movementOffsetMin;
	ImVec2 movementOffsetMax;

	ImVec2 position;
	ImVec2 size;

	CommonUtilities::Vector4<float> realPos;

	CommonUtilities::Vector4<float> tempMinMax;

	SpriteRenderer* mySpriteRenderer;
	std::shared_ptr<GameObject> mySprite;

	SpriteRenderer* myEdgeSpriteRenderer;
	std::shared_ptr<GameObject> myEdgeSprite;

	SpriteEditorBuffer spriteBuffer;

	bool creatingNewRect = false;
	int numberOfSprites = 0;

	int selectedSprite = -1;
	int hoveredSprite = -1;

	int cursor = 0;

	bool currentlyRescaling;
};