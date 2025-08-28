#pragma once

#include <functional>

#include <Vector/Vector2.hpp>

#include "Material/Material.h"

class GameObject;
struct ImFont;

class Editor
{
public:

	Editor();
	~Editor() = default;

	void Init();
	void Render();
	void Update();

	static Editor& Get();
	bool GetIsHoveringSceneWindow();
	
	CommonUtilities::Vector2i GetPositionSceneRelative(const CommonUtilities::Vector2i& aMin, const CommonUtilities::Vector2i& aMax, const CommonUtilities::Vector2i& aPosition);
	void LoadScene(const char* aPath, bool visual = true);
	void LoadSceneAsync(const char* aPath);

	void LoadSceneNetwork(const char* aPath, bool visual = true);

private:

	void SearchDirectory(const char* aPath, std::vector<std::pair<std::string, std::string>>& aCharList);

	void RenderTopMenuBar();
	void RenderInspector();
	void RenderHirearchy();
	void RenderScene();
	void RenderGame();

	void SaveDialougeScene();
	void SaveScene(const char* aPath);
	void LoadDialougeScene();
	
	void RenderSceneSelectedObject();
	void MoveSelectedSprite();

	void Shortcuts();

	void Paste();
	void Copy();
	void DeleteKey();

	std::function<void()> myLoadImGuiSettings;
	
private:

	bool mousePrev = false;

	std::vector<GameObject*>& mySelectedObjects;
	std::vector<CU::Vector2f> mySelectedObjectOffsets;

	std::vector<GameObject*> myCopiedObjects;

	Material myRectOutline;
	Material myRectPicking;

	unsigned rectPickingFrameBuffer;
	unsigned rectPickingTexture;

	unsigned sceneFrameBuffer;
	unsigned sceneTexture;

	ImFont* myComponentTitleFont;

	CommonUtilities::Vector2f minSceneView;
	CommonUtilities::Vector2f maxSceneView;

	bool myHoveringScne;
	bool myCurrentlmovingSprite;
	CU::Vector2f myMoveOffset;

	bool myMoveSnapping;
	bool myRotationSnapping;
	bool myScaleSnapping;

	float myMoveSnapStep;
	float myRotationSnapStep;
	float myScaleSnapStep;
};
