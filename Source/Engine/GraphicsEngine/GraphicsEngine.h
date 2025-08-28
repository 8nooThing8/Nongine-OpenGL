#pragma once

#include <Vector/Vector.hpp>
#include <Matrix/Matrix.hpp>
#include <unordered_map>
#include <vector>

#include "GraphicsCommandList.h"
#include "Sprite.h"

#include "Material/Material.h"

#include <sstream>
#include "../GameEngine/Components/GameObject.h"

class MeshRenderer;
class Sprite2DRenderer;
class GameObject;

struct SpriteRender
{
	SpriteRender() = default;

	SpriteRender(CU::Matrix4x4<float>& aTransform, CU::Vector3f& aPixelPickID) : transform(aTransform), pixelPickID(aPixelPickID)
	{
		
	}

	CU::Matrix4x4<float> transform;
	CU::Vector3f pixelPickID;

	bool isVisible;
};

class GraphicsEngine
{
public:
	//void SetVectorLayoutBuffer();
	//void SetMatrixLayoutBuffer();

	GraphicsEngine();
	~GraphicsEngine() = default;

	void Init();

	static GraphicsEngine& Get();
	GraphicsCommandList& GetCommandList();

	void PushSpriteRender(Sprite2DRenderer* aMeshrenderer);
	void PushForwardRender(MeshRenderer* aMeshrenderer);
	void PushDefferedRender(MeshRenderer* aMeshrenderer);

	void ClearAllrendererObjects();

	void LobbyDisplay();
	void FPSDisplay();

	void Render();

	void ImGuiBegin();
	void ImGuiRender();
	void ImGuiEnd();

	void SetResolution(const CU::Vector2<unsigned>& aResolution);
	const CU::Vector2<unsigned>& GetResolution();

	const non::Sprite& GetSprite() { return mySprite; }

	CU::Vector4u ReadPixel(const CU::Vector2i& aPos);
	void RenderPixelpicking(bool forced = false);

	float GetFPS();
	float GetAverageFPS();

private:
	void RenderSorting();

	void RenderShadows();
	void RenderSprites(bool isPixelpick = false);
	void RenderOutline();


	template <class T>
	void Render();

private:

	float fpsAvrage = 60.f;
	float fps = 60.f;

	bool mousePrev;

	float keptFPS;

	std::stringstream informationStream;

	non::Sprite mySprite;

	GraphicsCommandList myCommandList;

	unsigned previousBoundFramBuffer = 0;

	Material myOuline2DMaterial;
	unsigned myOuline2DRenderTarget;
	unsigned myOuline2DTexture;

	Material myFullscreenPixelpickMaterial;
	unsigned myFullscreenPixelpickTarget;
	unsigned myFullscreenPixelpickTexture;

	Material myPixelPickingMaterial;
	unsigned myPixelPickingRenderTarget;
	unsigned myPixelPickingTexture;

	// Diffuse ID
	std::unordered_map<int, std::pair<Sprite2DRenderer*, std::vector<SpriteRender*>>> mySpriteObjects;

	std::vector<MeshRenderer*> myForwardObjects;
	std::unordered_map<int, std::vector<MeshRenderer*>> myForwardObjectsToRemove;

	std::vector<MeshRenderer*> myDefferedObjects;

	CU::Vector2<unsigned> myResolution;
};