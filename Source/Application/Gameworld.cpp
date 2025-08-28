#include "Gameworld.h"

#include <InputHandler.h>

#include "ShaderCompiler.h"
#include "Material/Material.h"

#include "Components/GameObject.h"

#include <Timer.h>

#include <iostream>

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_glfw.h>
#include <ImGui/imgui_impl_opengl3.h>

#include "Engine/GameEngine/Inspector2DCamera.h"
#include "Engine/GameEngine/Camera.h"
#include "GameCamera.h"
#include "InspectorCamera.h"

#ifdef NONWIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#elif NONUNIX
#define GLFW_EXPOSE_NATIVE_X11
#elif NONEMSCRIPTEN
// #define GLFW_EXPOSE_NATIVE_X11
#endif

#include "ModelSprite/ModelFactory.h"

#include "Components/MeshRenderer.h"
#include "Components/Sprite2DRenderer.h"

#include "GraphicsCommandList.h"
#include "GraphicsEngine.h"

#include "Engine/GameEngine/Components/Game/PlayerMovement.h"
#include "Components/Physics2D/Colliders/BoxCollider2D.h"

#include "../PhysicsEngine/PhysicsEngine2D.h"
#include "Engine/GameEngine/Components/Physics2D/RigidBody2D.h"

#include "Application/editor.h"

#ifdef USENET
#include "Engine/GameEngine/Components/Networking/NetworkTransform.h"
#include "NetworkManager.h"
#include "Networking/Client/Client.h"
#endif

#ifdef NONEMSCRIPTEN
#include <emscripten/emscripten.h>
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <GLFW/glfw3native.h>

#ifndef IMGUISETTINGSDIR
#define IMGUISETTINGSDIR ""
#endif

GameWorld::GameWorld() = default;

void FrameBufferSizeCallBack(GLFWwindow * /*window*/, int width, int height)
{
	glViewport(0, 0, width, height);
	CU::Vector2<unsigned> resolution(width, height);
	GraphicsEngine::Get().SetResolution(resolution);
}

bool GameWorld::Initialize()
{
	SETUP_INPUT_HANDLER

	// Init GLFW
	{
		// SIMPLIFIED INITIALIZATION FIRST
#ifdef __EMSCRIPTEN__
		EM_ASM(
			console.log('Emscripten initialization starting...'););
#endif

		if (!glfwInit())
		{
			std::cout << "GLFW Init failed" << std::endl;
			return false;
		}

#ifdef __EMSCRIPTEN__
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
#else
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

		// Create simple window first
		MainSingleton::Get().myWindow = glfwCreateWindow(800, 600, "Nongine", NULL, NULL);
		if (!MainSingleton::Get().myWindow)
		{
			std::cout << "Window creation failed" << std::endl;
			glfwTerminate();
			return false;
		}

		glfwMakeContextCurrent(MainSingleton::Get().myWindow);

#ifdef __EMSCRIPTEN__
		EM_ASM(
			console.log('GLFW window and context created successfully'););
#endif

		// ONLY AFTER SUCCESSFUL CONTEXT CREATION, add the rest:
		glfwSetFramebufferSizeCallback(MainSingleton::Get().myWindow, FrameBufferSizeCallBack);
	}

	// Init GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	#ifdef __EMSCRIPTEN__
		EM_ASM(
			console.log('Glad fininshed'););
#endif

	// Imgui setup
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO &io = ImGui::GetIO();
		io.IniFilename = NULL;

		ImGui::LoadIniSettingsFromDisk(IMGUISETTINGSDIR);

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(MainSingleton::Get().myWindow, true);
		ImGui_ImplOpenGL3_Init("#version 460");
	}

	#ifdef __EMSCRIPTEN__
		EM_ASM(
			console.log('Imgui done'););
#endif

	GraphicsEngine::Get().Init();

	#ifdef __EMSCRIPTEN__
		EM_ASM(
			console.log('Graphicsengine Done'););
#endif

	// if (!PhysicsEngine::Get().Initialize())
	//{
	//	std::cout << "phyicsengine failed to initilize" << std::endl;
	// }

	Editor::Get().Init();

	return true;
}

void GameWorld::PingUpdate()
{
#ifdef USENET
	if (pingCheckTimer <= 0)
	{
		pingCheckTimer = 0.5f;
		if (!myClient->IsConnected())
			myClient->SendData(&pingCheckTimer, 1, DataTypeSent::CONNECT, MainSingleton::Get().myNetworkManager->myLobbyPartOff, 0, true);
		else
			myClient->SendData(&pingCheckTimer, 1, DataTypeSent::CONNECTIONINFOCLIENT, MainSingleton::Get().myNetworkManager->myLobbyPartOff, 0, true);
	}
	else
	{
		pingCheckTimer -= CU::Timer::GetDeltaTime();
	}
#endif
}

bool myLastPPressed = false;

void MainLoop()
{
// 	glfwPollEvents();

// #ifdef USENET
// 	PingUpdate();
// 	while (MainSingleton::Get().myNetworkQueue.size())
// 	{
// 		MainSingleton::Get().myNetworkQueue.front()();
// 		MainSingleton::Get().myNetworkQueue.pop();
// 	}
// #endif

// 	// non::PhysicsEngine2D::Get().SetGravity({0, 0.00981f});
// 	non::PhysicsEngine2D::Get().Update(CommonUtilities::Timer::GetDeltaTime());

 	GraphicsEngine::Get().ImGuiBegin();

// 	bool myPPressed = glfwGetKey(MainSingleton::Get().myWindow, GLFW_KEY_P) == GLFW_PRESS;

// 	// Player
// 	if (myPPressed && !myLastPPressed)
// 	{
// 		Material mat("Assets/Shaders/DefaultSpriteFS.glsl", "Assets/Shaders/DefaultSpriteVS.glsl");
// 		GameObject *go = new GameObject("Player");
// 		go->transform->SetScale(CU::Vector3f(0.1f, 0.1f, 0.1f));
// 		auto spriteRenderer = go->AddComponent<Sprite2DRenderer>(mat, 0);
// 		spriteRenderer->GetMaterial().SetTexture("Assets/Sprites/Person.png", DIFFUSE, SamplingType::Point);

// 		go->AddComponent<PlayerMovement>();
// 		go->AddComponent<non::RigidBody2D>(0.005f);
// 		go->AddComponent<non::BoxCollider2D>(CU::Vector2f{0.05f, 0.05f});
// #ifdef USENET
// 		go->AddComponent<NetworkTransform>()->recieveData = false;
// #endif
// 		go->StartMyComponents();

// 		MainSingleton::Get().myGameObjects.emplace(go->GetRawID(), go);

// #ifdef USENET
// 		MainSingleton::Get().myNetworkManager->GetNetRole()->SendData(&mat, 1, DataTypeSent::CREATEPLAYER, MainSingleton::Get().myNetworkManager->myLobbyPartOff, go->GetRawID(), true);
// #endif
// 	}

// 	myLastPPressed = myPPressed;

// 	static bool deleted = false;

// 	std::vector<int> idsToDelete;
// 	auto &gameobjects = MainSingleton::Get().myGameObjects;
// 	for (auto &[id, gameobject] : gameobjects)
// 	{
// 		if (gameobject->GetToBeDeleted())
// 		{
// 			delete gameobject;
// 			idsToDelete.emplace_back(id);
// 		}
// 	}
// 	for (auto &id : idsToDelete)
// 	{
// 		gameobjects.erase(id);
// 	}

// 	if (deleted)
// 	{
// 		MainSingleton::Get().myComponentManager->DeleteComponents();
// 		GraphicsEngine::Get().ClearAllrendererObjects();
// 		MainSingleton::Get().myComponentManager->AddSpritesToGraphicsEngine();

// 		deleted = false;
// 	}

// #ifdef USENET
// 	MainSingleton::Get().myNetworkManager->Update();
// #endif

// 	CommonUtilities::Timer::Update();
// 	InputHandler::GetInstance()->UpdateInput();

// 	Editor::Get().Update();

// 	CommonUtilities::GameCamera::main->Update(CommonUtilities::Timer::GetDeltaTime());

// 	// #ifndef NONEMSCRIPTEN
// 	// 	static bool fullScreenWindow = false;
// 	// 	if (glfwGetKey(MainSingleton::Get().myWindow, GLFW_KEY_F11) == GLFW_PRESS)
// 	// 	{
// 	// 		if (fullScreenWindow)
// 	// 		{
// 	// 			glfwSetWindowMonitor(MainSingleton::Get().myWindow, nullptr, fullscreenMode->width / 2 - windowedMode.x / 2, fullscreenMode->height / 2 - windowedMode.y / 2, windowedMode.x, windowedMode.y, GLFW_DONT_CARE);
// 	// 			GraphicsEngine::Get().SetResolution(windowedMode);
// 	// 		}
// 	// 		else
// 	// 		{
// 	// 			glfwSetWindowMonitor(MainSingleton::Get().myWindow, monitor, 0, 0, fullscreenMode->width, fullscreenMode->height, fullscreenMode->refreshRate);
// 	// 			CU::Vector2<unsigned> resolution(fullscreenMode->width, fullscreenMode->height);
// 	// 			GraphicsEngine::Get().SetResolution(resolution);
// 	// 		}

// 	// 		fullScreenWindow = !fullScreenWindow;
// 	// 	}
// 	// #endif

// 	MainSingleton::Get().myComponentManager->EarlyUpdateComponents(CommonUtilities::Timer::GetDeltaTime());
// 	MainSingleton::Get().myComponentManager->UpdateComponents(CommonUtilities::Timer::GetDeltaTime());
// #ifdef USENET
// 	if (MainSingleton::Get().myNetworkManager->createObject)
// 	{
// 		glfwMakeContextCurrent(MainSingleton::Get().myWindow);
// 		MainSingleton::Get().myNetworkManager->createObject = false;
// 	}

// 	myClient->CommitDataSend();
// #endif

 	GraphicsEngine::Get().ImGuiRender();
// 	Editor::Get().Render();
 	GraphicsEngine::Get().ImGuiEnd();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(MainSingleton::Get().myWindow);
}

int GameWorld::Run()
{
	srand(time(NULL));

	Inspector2DCamera inspectorCamera;

	CommonUtilities::GameCamera::main->Update(CommonUtilities::Timer::GetDeltaTime());

	glEnable(GL_BLEND);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glCullFace(GL_NONE);

	glfwWindowHint(GLFW_SAMPLES, 4);
	Editor::Get().LoadScene("scene");

#ifdef USENET
	MainSingleton::Get().myNetworkManager->StartGameClient();
	myClient = reinterpret_cast<Client *>(MainSingleton::Get().myNetworkManager->GetNetRole());
#endif

#ifndef NONEMSCRIPTEN
	while (myIsRunning)
	{
		MainLoop();
	}
#else
	emscripten_set_main_loop(MainLoop, 0, 1);
	emscripten_set_main_loop_timing(EM_TIMING_RAF, 1); // <-- after main loop
#endif
	// Cleanup
	// ImGui_ImplOpenGL3_Shutdown();
	// ImGui_ImplGlfw_Shutdown();
	// ImGui::DestroyContext();

	// glfwDestroyWindow(MainSingleton::Get().myWindow);
	// glfwTerminate();
	return 0;
}