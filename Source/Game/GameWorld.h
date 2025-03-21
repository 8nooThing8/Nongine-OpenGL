#pragma once

#include <vector>
#include <memory>

#include "string"

#include <Vector\Vector2.hpp>
#include <Timer.h>

#include "../Engine/GameEngine/AssetsTab.h"
#include "../Engine/GameEngine/DefferedRendering.h"

#include "../Engine/GameEngine/GameCamera.h"
#include "../Engine/GameEngine/Components/CameraComponent.h"

struct ImGuiIO;
class GameObject;

class InspectorCamera;

struct Scene;

class GameWorld
{
public:
	GameWorld(HWND aHWND);

	~GameWorld() = default;

	void SetActive(bool aActive);

	void RenderHierarchy();
	void RenderInspector();
	void TopFileMenuItem();

	void RenderTopMenuBar();

	void TopNewObjectMenuItem();

	void ShortCuts();

	void Begin();

	void EditorUpdate();

	void EditorRenderGUI();
	void SampleSceneCreation();

private:
	void ChildChecker(std::vector<std::shared_ptr<GameObject>>& aChildren, int aTotalIndent);
	void RenderImGui(ImGuiIO& /*io*/);

	void EnterPlayMode();
	void ExitPlayMode();

	void SceneRender();

public:

private:
	CommonUtilities::Vector2<int> mousePos;

	float nextFPSUpdate = 0;
	float currentDisplayedFPS = 0;

	AssetsTab myAssetsTab;

	GameCamera* inspectorCamera;
	CameraComponent* gameCamera;
};