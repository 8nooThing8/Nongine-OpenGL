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

	std::shared_ptr<GameObject> NewGameObject(const std::string& aName, std::string& aPath) const;
	std::shared_ptr<GameObject> NewEmptyGameObject(const std::string& aName) const;

	void RenderHierarchy();
	void RenderInspector();
	void TopFileMenuItem();

	void RenderTopMenuBar();

	void TopNewObjectMenuItem();

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

	DefferedRendering myDeffRendering;
};