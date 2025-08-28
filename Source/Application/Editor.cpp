#include "editor.h"

#include <InputHandler.h>

#include "../Engine/GameEngine/MainSingleton.h"
#include "../Engine/GameEngine/Components/Transform.h"

#include <string>

#include <Engine/GraphicsEngine/GraphicsEngine.h>
#include <ImGui/imgui.h>

#include "Engine/GameEngine/Components/Physics2D/Colliders/BoxCollider2D.h"

#include "Engine/GameEngine/Components/Sprite2DRenderer.h"
#include "Engine/GameEngine/Components/Game/PlayerMovement.h"
#include "Engine/GameEngine/Components/GameObject.h"
#include <Engine/GraphicsEngine/GraphicCommands/RenderSpriteCommand.h>

#include <Engine/GameEngine/Inspector2DCamera.h>
#include <Engine/GameEngine/GameCamera.h>

#ifdef USENET
#include "Engine/GameEngine/Components/Networking/NetworkTransform.h"
#include "Engine/GameEngine/NetworkManager.h"
#endif

#include <iostream>
#include <fstream>

#include <filesystem>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#ifndef ZeroMemory
#define ZeroMemory(Destination, Length) std::memset((Destination), 0, (Length))
#endif

using namespace CommonUtilities;

Editor::Editor() : mySelectedObjects(MainSingleton::Get().mySelectedObjects)
{
}

void Editor::Init()
{
	glGenFramebuffers(1, &sceneFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, sceneFrameBuffer);

	glGenTextures(1, &sceneTexture);
	glBindTexture(GL_TEXTURE_2D, sceneTexture);
	// Change this to when it scales
	auto resolution = GraphicsEngine::Get().GetResolution();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	#ifdef __EMSCRIPTEN__
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneTexture, 0);
#else
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, sceneTexture, 0);
#endif

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";

	glGenFramebuffers(1, &rectPickingFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, rectPickingFrameBuffer);

	glGenTextures(1, &rectPickingTexture);
	glBindTexture(GL_TEXTURE_2D, rectPickingTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		#ifdef __EMSCRIPTEN__
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rectPickingTexture, 0);
#else
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, rectPickingTexture, 0);
#endif

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
		
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	auto io = ImGui::GetIO();
	// io.Fonts->AddFontFromFileTTF("Assets/Fonts/Comic Sans MS.ttf", 20);
	// myComponentTitleFont = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Comic Sans MS.ttf", 32);

	myRectOutline = Material("Assets/Shaders/RectOutlineFS.glsl", "Assets/Shaders/NonInstancedSpriteVS.glsl", false);
	myRectPicking = Material("Assets/Shaders/RectPickingFS.glsl", "Assets/Shaders/NonInstancedSpriteVS.glsl", false);
}

void Editor::Render()
{
	RenderTopMenuBar();

	ImGui::Begin("Hirearchy");
	RenderHirearchy();
	ImGui::End();

	ImGui::Begin("Inspector");
	RenderInspector();
	ImGui::End();

	RenderScene();

	ImGui::Begin("Game");
	RenderGame();
	ImGui::End();
}

void Editor::Update()
{
	Shortcuts();
}

Editor &Editor::Get()
{
	static Editor editor;
	return editor;
}

bool Editor::GetIsHoveringSceneWindow()
{
	return myHoveringScne;
}

void Editor::RenderTopMenuBar()
{
	ImGui::BeginMainMenuBar();

	if (ImGui::MenuItem("New"))
	{
		Material mat("Assets/Shaders/DefaultSpriteFS.glsl", "Assets/Shaders/DefaultSpriteVS.glsl");
		mat.SetTexture("Assets/Sprites/Person.png", ALBEDO, Point);
		GameObject *go = new GameObject("person");
		go->AddComponent<Sprite2DRenderer>(mat, 0);
		go->transform->SetScale(CU::Vector3f{0.1f, 0.1f, 0.1f});
		MainSingleton::Get().myGameObjects.emplace(go->GetRawID(), std::move(go));

		go->AddComponent<non::BoxCollider2D>(CU::Vector2f{0.05f, 0.05f});

		go->StartMyComponents();
	}

	else if (ImGui::MenuItem("Save"))
		SaveDialougeScene();
	else if (ImGui::MenuItem("Load"))
		LoadDialougeScene();
	else if (ImGui::MenuItem("LOAD IMGUI Settings"))
		myLoadImGuiSettings();
	else if (ImGui::MenuItem("SAVE IMGUI Settings"))
		ImGui::SaveIniSettingsToDisk(IMGUISETTINGSDIR);

	ImGui::EndMainMenuBar();
}

void Editor::RenderInspector()
{
	if (mySelectedObjects.empty())
		return;

	RenderSceneSelectedObject();

	const auto &selectedObject = mySelectedObjects.front();

	if (!selectedObject)
		return;

	// Transform
	{
		//ImGui::PushFont(myComponentTitleFont);
		ImGui::Text("Transform");
		ImGui::PopFont();

		ImGui::Dummy(ImVec2(16, 0));
		ImGui::SameLine();

		ImGui::BeginGroup();

		CU::Vector4f position = selectedObject->transform->GetPosition();
		ImGui::Text("Position");
		ImGui::SameLine();
		ImGui::Dummy(ImVec2(ImGui::CalcTextSize("Rotation ").x - ImGui::CalcTextSize("Position").x, 0));
		ImGui::SameLine();
		if (ImGui::DragFloat2("##Position", &position[0], 0.01f))
		{
			selectedObject->transform->SetIsDirty();
		}
		selectedObject->transform->SetPosition(position.ToVector3());

		float rotation = selectedObject->transform->GetRotation().z * rad2deg;
		ImGui::Text("Rotation");
		ImGui::SameLine();
		ImGui::Dummy(ImVec2(ImGui::CalcTextSize("Rotation ").x - ImGui::CalcTextSize("Rotation").x, 0));
		ImGui::SameLine();
		if (ImGui::DragFloat("##rotation", &rotation, 0.1f))
		{
			selectedObject->transform->SetIsDirty();
		}
		selectedObject->transform->SetRotation(CU::Vector3f(0, 0, rotation * deg2rad));

		CU::Vector4f scale = selectedObject->transform->GetScale();
		ImGui::Text("Scale");
		ImGui::SameLine();
		ImGui::Dummy(ImVec2(ImGui::CalcTextSize("Rotation ").x - ImGui::CalcTextSize("Scale").x, 0));
		ImGui::SameLine();
		if (ImGui::DragFloat2("##scale", &scale[0], 0.1f))
		{
			selectedObject->transform->SetIsDirty();
		}
		selectedObject->transform->SetScale(scale.ToVector3());

		ImGui::EndGroup();
	}

	// Sprite render
	{
		//ImGui::PushFont(myComponentTitleFont);
		ImGui::Text("Sprite 2D Renderer");
		ImGui::PopFont();

		ImGui::Dummy(ImVec2(16, 0));
		ImGui::BeginGroup();

		if (ImGui::BeginCombo("Texture", selectedObject->GetComponent<Sprite2DRenderer>()->GetMaterial().GetTextureName()))
		{
			std::vector<std::pair<std::string, std::string>> myTextures;

			SearchDirectory("Assets", myTextures);
			for (const auto &texture : myTextures)
				if (ImGui::Selectable(texture.first.c_str()))
					selectedObject->GetComponent<Sprite2DRenderer>()->GetMaterial().SetTexture(texture.second.c_str(), 0);

			ImGui::EndCombo();
		}

		ImGui::EndGroup();
	}
}

void Editor::SearchDirectory(const char *aPath, std::vector<std::pair<std::string, std::string>> &aCharList)
{
	for (const auto &entry : std::filesystem::recursive_directory_iterator(aPath))
	{
		std::filesystem::path path = entry.path();

		if (path.extension() == ".png")
		{
			std::filesystem::path name = entry.path();

			name.replace_extension();
			aCharList.emplace_back(std::pair(name.filename().string(), path.string()));
		}
	}

	std::cout << std::endl
			  << std::endl
			  << std::endl;
}

void Editor::RenderHirearchy()
{
}

void Editor::RenderScene()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	ImGui::Begin("Scene");

	bool test = true;

	ImGui::Checkbox("MoveSnap", &myMoveSnapping);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50);
	ImGui::InputFloat("SnapStep##Pos", &myMoveSnapStep);

	ImGui::SameLine();
	ImGui::Checkbox("RotationSnap", &myRotationSnapping);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50);
	ImGui::InputFloat("SnapStep##Rotation", &myRotationSnapStep);

	ImGui::SameLine();
	ImGui::Checkbox("ScaleSnap", &myScaleSnapping);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50);
	ImGui::InputFloat("SnapStep##Scale", &myScaleSnapStep);

	glBindFramebuffer(GL_FRAMEBUFFER, sceneFrameBuffer);
	GraphicsEngine::Get().Render();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	const float window_width = ImGui::GetContentRegionAvail().x;
	const float window_height = window_width * 0.5625f;

	ImVec2 pos = ImGui::GetCursorScreenPos();

	float windowPosY = ImGui::GetWindowSize().y * 0.5f - window_height * 0.5f - 12.f;

	minSceneView = CU::Vector2f(pos.x, pos.y + windowPosY);
	maxSceneView = CU::Vector2f(pos.x + window_width, pos.y + window_height + windowPosY);

	ImGui::GetForegroundDrawList()->AddRect(ImVec2(minSceneView.x, minSceneView.y), ImVec2(maxSceneView.x, maxSceneView.y), IM_COL32(100, 100, 100, 255));

	ImGui::GetWindowDrawList()->AddImage(
		sceneTexture,
		ImVec2(pos.x, pos.y + windowPosY),
		ImVec2(pos.x + window_width, pos.y + window_height + windowPosY),
		ImVec2(0, 1),
		ImVec2(1, 0));

	myHoveringScne = ImGui::IsWindowHovered();

	ImGui::End();

	ImGui::PopStyleVar();
}

void Editor::RenderGame()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	ImGui::PopStyleVar();
}

void Editor::SaveDialougeScene()
{
	SaveScene("Scene");
}

const size_t transformByteCount = 3 * sizeof(CU::Vector3f);
const size_t SpriteRendererByteCount = 128;

void Editor::SaveScene(const char *aPath)
{
	auto& gameObjects = MainSingleton::Get().myGameObjects;

	std::ofstream saveStream;
	saveStream.open(aPath, std::ios::binary);

	char goCount[sizeof(size_t)];
	size_t size = gameObjects.size();
	std::memcpy(goCount, &size, sizeof(size_t));
	saveStream.write(goCount, sizeof(size_t));

	// char prefabPivot[sizeof(CU::Vector3f)];
	// size_t size = gameObjects.size();
	// std::memcpy(goCount, &size, sizeof(size_t));
	// saveStream.write(goCount, sizeof(size_t));

	for (const auto &gameobject : gameObjects)
	{
		char transformData[transformByteCount];
		std::memcpy(transformData, &gameobject.second->transform->myPosition, sizeof(CU::Vector3f));
		std::memcpy(transformData + sizeof(CU::Vector3f), &gameobject.second->transform->myRotation, sizeof(CU::Vector3f));
		std::memcpy(transformData + sizeof(CU::Vector3f) * 2, &gameobject.second->transform->myScale, sizeof(CU::Vector3f));
		saveStream.write(transformData, transformByteCount);

		char spriteRendererData[SpriteRendererByteCount];
		std::memcpy(spriteRendererData, gameobject.second->GetComponent<Sprite2DRenderer>()->GetMaterial().GetTexturePath(), SpriteRendererByteCount);
		saveStream.write(spriteRendererData, SpriteRendererByteCount);
	}

	saveStream.close();
}

void Editor::LoadDialougeScene()
{
	LoadScene("Scene");
	// LoadSceneAsync("Scene");
}

void Editor::LoadSceneAsync(const char *aPath)
{
	auto& gameObjects = MainSingleton::Get().myGameObjects;

	std::ifstream loadStream;
	loadStream.open(aPath, std::ios::binary);

	if (loadStream.is_open())
	{
		mySelectedObjects.clear();

		char data[65535];
		ZeroMemory(data, 8);

		loadStream.read(data, 65535);

		size_t objectCount = 0;
		std::memcpy(&objectCount, data, sizeof(size_t));

		for (int i = 0; i < objectCount; i++)
		{
			Material mat("Assets/Shaders/DefaultSpriteFS.glsl", "Assets/Shaders/DefaultSpriteVS.glsl");
			GameObject *go = new GameObject("obj");
			auto spriteRenderer = go->AddComponent<Sprite2DRenderer>(mat, 0);
			gameObjects.emplace(go->GetRawID(), go);

			int memoryOffset = (i * (transformByteCount + SpriteRendererByteCount)) + 8;

			std::memcpy(&go->transform->myPosition, data + memoryOffset, sizeof(CU::Vector3f));
			std::memcpy(&go->transform->myRotation, data + memoryOffset + sizeof(CU::Vector3f), sizeof(CU::Vector3f));
			std::memcpy(&go->transform->myScale, data + memoryOffset + sizeof(CU::Vector3f) * 2, sizeof(CU::Vector3f));

			char texturePath[SpriteRendererByteCount];
			std::memcpy(texturePath, data + transformByteCount + memoryOffset, SpriteRendererByteCount);
			spriteRenderer->GetMaterial().SetTexture(texturePath, DIFFUSE, SamplingType::Point);
		}

		loadStream.close();

		MainSingleton::Get().myComponentManager->StartComponents();

		return;
	}
}
void Editor::LoadScene(const char *aPath, bool visual)
{
	auto& gameObjects = MainSingleton::Get().myGameObjects;

	std::ifstream loadStream;
	loadStream.open(aPath, std::ios::binary);

	if (loadStream.is_open())
	{
		GameObject::nextID = 500;
		mySelectedObjects.clear();

		for (auto& obj : gameObjects)
			delete obj.second;

		gameObjects.clear();
		GraphicsEngine::Get().ClearAllrendererObjects();
		MainSingleton::Get().myComponentManager->myComponents.clear();

		char data[65535] = "\0";
		ZeroMemory(data, 8);

		loadStream.read(data, 65535);

		size_t objectCount = 0;
		std::memcpy(&objectCount, data, sizeof(size_t));

		for (int i = 0; i < objectCount; i++)
		{
			GameObject *go = new GameObject("obj");

			int memoryOffset = (i * (transformByteCount + SpriteRendererByteCount)) + 8;

			std::memcpy(&go->transform->myPosition, data + memoryOffset, sizeof(CU::Vector3f));
			std::memcpy(&go->transform->myRotation, data + memoryOffset + sizeof(CU::Vector3f), sizeof(CU::Vector3f));
			std::memcpy(&go->transform->myScale, data + memoryOffset + sizeof(CU::Vector3f) * 2, sizeof(CU::Vector3f));

			Material mat("Assets/Shaders/DefaultSpriteFS.glsl", "Assets/Shaders/DefaultSpriteVS.glsl");
			auto spriteRenderer = go->AddComponent<Sprite2DRenderer>(mat, 0);

			char texturePath[SpriteRendererByteCount];
			std::memcpy(texturePath, data + transformByteCount + memoryOffset, SpriteRendererByteCount);
			spriteRenderer->GetMaterial().SetTexture(texturePath, DIFFUSE, SamplingType::Point);

			/*auto netTransform = go->AddComponent<NetworkTransform>();
			netTransform->sendData = false;
			netTransform->recieveData = true;*/

			gameObjects.emplace(go->GetRawID(), go);
		}

		loadStream.close();

		MainSingleton::Get().myComponentManager->StartComponents();

		return;
	}
}

void Editor::LoadSceneNetwork(const char *aPath, bool visual)
{
	aPath;
	visual;

	#ifdef USENET
	auto& gameObjects = MainSingleton::Get().myGameObjects;

	std::ifstream loadStream;
	loadStream.open(aPath, std::ios::binary);

	if (loadStream.is_open())
	{
		GameObject::nextID = 500;
		mySelectedObjects.clear();

		for (auto& obj : gameObjects)
			delete obj.second;

		gameObjects.clear();
		GraphicsEngine::Get().ClearAllrendererObjects();
		MainSingleton::Get().myComponentManager->myComponents.clear();

		char data[65535] = "\0";
		ZeroMemory(data, 8);

		loadStream.read(data, 65535);

		size_t objectCount = 0;
		std::memcpy(&objectCount, data, sizeof(size_t));

		for (int i = 0; i < objectCount; i++)
		{
			GameObject *go = new GameObject("obj");

			int memoryOffset = (i * (transformByteCount + SpriteRendererByteCount)) + 8;

			std::memcpy(&go->transform->myPosition, data + memoryOffset, sizeof(CU::Vector3f));
			std::memcpy(&go->transform->myRotation, data + memoryOffset + sizeof(CU::Vector3f), sizeof(CU::Vector3f));
			std::memcpy(&go->transform->myScale, data + memoryOffset + sizeof(CU::Vector3f) * 2, sizeof(CU::Vector3f));

			go->AddComponent<NetworkTransform>();
			int val = 1;

			#ifdef USENET
				MainSingleton::Get().myNetworkManager->GetNetRole()->SendData(&val, 1, DataTypeSent::CREATEGAMEOBJECT, MainSingleton::Get().myNetworkManager->myLobbyPartOff, go->GetRawID());
			#endif
			
			/*auto netTransform = go->AddComponent<NetworkTransform>();
			netTransform->sendData = false;
			netTransform->recieveData = true;*/

			gameObjects.emplace(go->GetRawID(), go);
		}

		loadStream.close();

		MainSingleton::Get().myComponentManager->StartComponents();

		return;
	}
	#endif
}

void Editor::RenderSceneSelectedObject()
{
	for (const auto &selectedObject : mySelectedObjects)
		GraphicsEngine::Get().GetCommandList().Enqueue<RenderSpriteCommand>(selectedObject, &myRectOutline, false);

	GraphicsEngine::Get().GetCommandList().Enqueue([&]{ 
		glBindFramebuffer(GL_FRAMEBUFFER, rectPickingFrameBuffer); 
		glClearColor(0, 0, 0, 0); 
		glClear(GL_COLOR_BUFFER_BIT); });

	for (const auto &selectedObject : mySelectedObjects)
	{
		GraphicsEngine::Get().GetCommandList().Enqueue<RenderSpriteCommand>(selectedObject, &myRectPicking, false);
	}
	
	bool mouseCurrent = (glfwGetMouseButton(MainSingleton::Get().myWindow, 0) == GLFW_PRESS);

	if (myHoveringScne && (mouseCurrent && !mousePrev))
	{
		GraphicsEngine::Get().GetCommandList().Enqueue([&]
			{
			CU::Vector2d mousePos;
			glfwGetCursorPos(MainSingleton::Get().myWindow, &mousePos.x, &mousePos.y);
			
			CU::Vector2i windowPos;
			glfwGetWindowPos(MainSingleton::Get().myWindow, &windowPos.x, &windowPos.y);
			
			CU::Vector2i windowSize;
			glfwGetWindowSize(MainSingleton::Get().myWindow, &windowSize.x, &windowSize.y);

			Vector2i min{ windowPos.x,  windowPos.y };
			Vector2i max{ windowPos.x + windowSize.x, windowPos.y + windowSize.y };

			auto pos = Editor::Get().GetPositionSceneRelative(
				CU::Vector2i(min.x, min.y),
				CU::Vector2i(max.x, max.y),
				CU::Vector2i(mousePos.x, mousePos.y));

			auto val = GraphicsEngine::Get().ReadPixel(CU::Vector2i(pos.x, 1080 - pos.y));

			int pickedID = val.x + val.y * 256 + val.z * 256 * 256;

			if (pickedID != 0)
			{
				myCurrentlmovingSprite = true;

				mySelectedObjectOffsets.clear();
				for (int i = 0; i < mySelectedObjects.size(); i++)
				{
					const auto& selectedObject = mySelectedObjects[i];

					if (pickedID == selectedObject->GetRawID())
						std::swap(mySelectedObjects[i], mySelectedObjects[0]);

					const CU::Vector4f offsetPosition(selectedObject->transform->GetPosition() - mySelectedObjects.front()->transform->GetPosition());
					mySelectedObjectOffsets.emplace_back(offsetPosition.ToVector2());
				}
			}
			else
			{
				if (!InputHandler::GetKey(keycode::SHIFT))
				{
					mySelectedObjectOffsets.clear();
					mySelectedObjects.clear();
				}
				GraphicsEngine::Get().RenderPixelpicking(true);
			} });
	}

	mousePrev = mouseCurrent;

	if (myCurrentlmovingSprite)
	{
		MoveSelectedSprite();

		if (glfwGetMouseButton(MainSingleton::Get().myWindow, 0) == GLFW_RELEASE)
			myCurrentlmovingSprite = false;
	}

	GraphicsEngine::Get().GetCommandList().Enqueue([&]
												   { glBindFramebuffer(GL_FRAMEBUFFER, 0); });
}

void Editor::MoveSelectedSprite()
{
	ImGui::SetNextWindowPos(ImVec2(500, 500));

	CU::Vector2d mousePos;
	glfwGetCursorPos(MainSingleton::Get().myWindow, &mousePos.x, &mousePos.y);
			
	CU::Vector2i windowPos;
	glfwGetWindowPos(MainSingleton::Get().myWindow, &windowPos.x, &windowPos.y);
			
	CU::Vector2i windowSize;
	glfwGetWindowSize(MainSingleton::Get().myWindow, &windowSize.x, &windowSize.y);

	Vector2i min{ windowPos.x,  windowPos.y };
	Vector2i max{ windowPos.x + windowSize.x, windowPos.y + windowSize.y };

	CU::Vector2i pos = Editor::Get().GetPositionSceneRelative(
		CU::Vector2i(min.x, min.y),
		CU::Vector2i(max.x, max.y),
		CU::Vector2i(mousePos.x, mousePos.y));

	CU::Vector3f cameraPosition = GameCamera::main->position;

	float zoom = static_cast<Inspector2DCamera*>(GameCamera::main)->zoom;

	CU::Vector3f newPosition(pos.x / 1920.f - cameraPosition.x + myMoveOffset.x, 1 - (pos.y / 1080.f - cameraPosition.y + myMoveOffset.y), 0);

	for (int i = 0; i < mySelectedObjectOffsets.size(); i++)
	{
		const auto& currentObject = mySelectedObjects[i];
		const auto& currentOffset = mySelectedObjectOffsets[i];

		CU::Vector3f offsetNoZ = currentObject->transform->GetScale().ToVector3();
		offsetNoZ.z = 0;

		CU::Vector3f position = (newPosition / zoom) - offsetNoZ;
		position.x *= 1.7777777777f;

		CU::Vector3f finalPosition = position + CU::Vector3f((currentObject->transform->GetScale().x * 1.777777f) * (0.73f), currentObject->transform->GetScale().y * 0.5f, 0); 

		currentObject->transform->SetPosition(CU::Vector3f(roundf(finalPosition.x * 10) * 0.1f, roundf(finalPosition.y * 10) * 0.1f, 0) + CU::Vector3f(currentOffset.x, currentOffset.y, 0));

		#ifdef USENET
		if (currentObject->GetComponent<NetworkTransform>())
			currentObject->GetComponent<NetworkTransform>()->ForceUpdateTransform();
		#endif

	}
}

void Editor::DeleteKey()
{
	GraphicsEngine::Get().ClearAllrendererObjects();

	for (const auto &selectedObject : mySelectedObjects)
		selectedObject->Destroy();

	std::vector<int> idsToDelete;

	auto& gameobjects = MainSingleton::Get().myGameObjects;

	for (auto& [id, gameobject] : gameobjects)
	{
		if (gameobject->GetToBeDeleted())
		{
			delete gameobject;
			idsToDelete.emplace_back(id);
		}
	}
	for (auto& id : idsToDelete)
	{
		gameobjects.erase(id);
	}

	MainSingleton::Get().myComponentManager->DeleteComponents();
	MainSingleton::Get().myComponentManager->AddSpritesToGraphicsEngine();

	mySelectedObjects.clear();
	mySelectedObjectOffsets.clear();
}

void Editor::Copy()
{
	myCopiedObjects.clear();
	for (const auto &selectedObject : mySelectedObjects)
	{
		myCopiedObjects.emplace_back(selectedObject);
	}
}

void Editor::Paste()
{
	mySelectedObjects.clear();
	for (const auto &copiedObject : myCopiedObjects)
	{
		GameObject *go = new GameObject("obj");

		if (const auto &spriteRend = copiedObject->GetComponent<Sprite2DRenderer>())
			go->AddComponent<Sprite2DRenderer>(spriteRend->GetMaterial(), 0);

		std::memcpy(&go->transform->myPosition.x, &copiedObject->transform->myPosition.x, 3 * sizeof(CU::Vector4f));

		float totalYPos = 0;

		for (const auto &obj : myCopiedObjects)
		{
			totalYPos += obj->transform->GetScale().y;
		}

		go->transform->myPosition.y += totalYPos;

		MainSingleton::Get().myGameObjects.emplace(go->GetRawID(), go);

		go->StartMyComponents();

		mySelectedObjects.emplace_back(go);
	}
}

void Editor::Shortcuts()
{
	bool controlHeld = InputHandler::GetKey(keycode::CONTROL);

	if (controlHeld)
	{
		bool copy = InputHandler::GetKeyDown(keycode::C);
		bool paste = InputHandler::GetKeyDown(keycode::V);
		bool duplicate = InputHandler::GetKeyDown(keycode::D);

		if (copy)
		{
			Copy();
		}
		else if (paste)
		{
			Paste();
		}
		else if (duplicate)
		{
			Copy();
			Paste();
		}
	}
	else
	{
		bool deleteKey = InputHandler::GetKeyDown(keycode::DELETE_BUTTON);

		if (deleteKey)
		{
			DeleteKey();
		}
	}
}

CU::Vector2i Editor::GetPositionSceneRelative(const CU::Vector2i &aMin, const CU::Vector2i &aMax, const CU::Vector2i &aPosition)
{
	CU::Vector2i positionRelative;

	positionRelative.x = CU::MapValue<int>(aMin.x, aMax.x, static_cast<int>(minSceneView.x), static_cast<int>(maxSceneView.x), aPosition.x);
	positionRelative.y = CU::MapValue<int>(aMin.y, aMax.y, static_cast<int>(minSceneView.y), static_cast<int>(maxSceneView.y), aPosition.y);

	return positionRelative;
}
