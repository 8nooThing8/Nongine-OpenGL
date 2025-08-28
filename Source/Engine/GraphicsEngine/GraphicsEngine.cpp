#include <algorithm>
#include <InputHandler.h>

#include "GraphicsEngine.h"

#include "../Engine/GameEngine/GameCamera.h"

#include "../Engine/GameEngine/Components/MeshRenderer.h"
#include "../Engine/GameEngine/Components/Sprite2DRenderer.h"
#include "../Engine/GameEngine/Components/GameObject.h"
#include "../Engine/GameEngine/Components/Transform.h"

#include "GraphicCommands/RenderMeshCommand.h"
#include "GraphicCommands/RenderShadowCommand.h"
#include "GraphicCommands/RenderInstancedSpriteCommand.h"
#include <GraphicCommands/RenderSpriteCommand.h>

#include "../Engine/GameEngine/Components/DirectionalLight.h"

#include <ImGui/imgui.h>
#include <Timer.h>
#include <ImGui/imgui_impl_opengl3.h>
#include <ImGui/imgui_impl_glfw.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Material/Material.h"

#include "editor.h"

#ifdef USENET
#include "Engine/GameEngine/NetworkManager.h"
#include "Networking/Shared/NetRole.h"
#endif

void GraphicsEngine::Init()
{
#ifdef __EMSCRIPTEN__
	EM_ASM(
		console.log('Initing graphics engine'););
#endif

	ImGuiIO &imIO = ImGui::GetIO();

#ifdef __EMSCRIPTEN__
	EM_ASM(
		console.log('GetIO'););
#endif

	imIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	{
		glGenFramebuffers(1, &myPixelPickingRenderTarget);

		glBindFramebuffer(GL_FRAMEBUFFER, myPixelPickingRenderTarget);

		glGenTextures(1, &myPixelPickingTexture);
		glBindTexture(GL_TEXTURE_2D, myPixelPickingTexture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

#ifdef __EMSCRIPTEN__
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, myPixelPickingTexture, 0);
#else
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, myPixelPickingTexture, 0);
#endif

#ifdef __EMSCRIPTEN__
		EM_ASM(
			console.log('GL first buffers'););
#endif
		GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, DrawBuffers);
	}

#ifdef __EMSCRIPTEN__
	EM_ASM(
		console.log('Glbuffers'););
#endif

	{
		glGenFramebuffers(1, &myOuline2DRenderTarget);
		glBindFramebuffer(GL_FRAMEBUFFER, myOuline2DRenderTarget);

		glGenTextures(1, &myOuline2DTexture);
		glBindTexture(GL_TEXTURE_2D, myOuline2DTexture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		#ifdef __EMSCRIPTEN__
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, myOuline2DTexture, 0);
#else
glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, myOuline2DTexture, 0);
#endif

		GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, DrawBuffers);
	}



	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	#ifdef __EMSCRIPTEN__
	EM_ASM(
		console.log('bindframbuffer'););
#endif

	mySprite.Init();

	#ifdef __EMSCRIPTEN__
	EM_ASM(
		console.log('SpriteInit'););
#endif

	myFullscreenPixelpickMaterial = Material("Assets/Shaders/2DOutlineFS.glsl", "Assets/Shaders/QuadVS.glsl", false);
	myOuline2DMaterial = Material("Assets/Shaders/PixelPickingFS.glsl", "Assets/Shaders/NonInstancedSpriteVS.glsl");
	myPixelPickingMaterial = Material("Assets/Shaders/PixelPickingFS.glsl", "Assets/Shaders/DefaultSpriteVS.glsl");

#ifdef __EMSCRIPTEN__
	EM_ASM(
		console.log('Extra paths'););
#endif
}

GraphicsEngine &GraphicsEngine::Get()
{
	static GraphicsEngine GE;
	return GE;
}

GraphicsCommandList &GraphicsEngine::GetCommandList()
{
	return myCommandList;
}

void GraphicsEngine::PushSpriteRender(Sprite2DRenderer *aSpriterenderer)
{
	unsigned textureID = aSpriterenderer->GetMaterial().GetTexture(DIFFUSE).myID;

	auto &sprite = mySpriteObjects[textureID];
	sprite.second.emplace_back(&aSpriterenderer->renderer);
	sprite.first = {aSpriterenderer};
}
void GraphicsEngine::PushForwardRender(MeshRenderer *aMeshrenderer)
{
	if (aMeshrenderer->myLayerToRemove)
		myForwardObjectsToRemove[aMeshrenderer->myLayerToRemove].emplace_back(aMeshrenderer);
	else
		myForwardObjects.emplace_back(aMeshrenderer);
}

void GraphicsEngine::ClearAllrendererObjects()
{
	mySpriteObjects.clear();
	myForwardObjects.clear();
	myDefferedObjects.clear();
}

void GraphicsEngine::PushDefferedRender(MeshRenderer *aMeshrenderer)
{
	myDefferedObjects.emplace_back(aMeshrenderer);
}

void GraphicsEngine::RenderSorting()
{
	const CU::Vector4f &cameraPosition = CU::GameCamera::main->position;

	std::sort(myForwardObjects.begin(), myForwardObjects.end(), [&cameraPosition](MeshRenderer *aMeshRenderFirst, MeshRenderer *aMeshRendererSecond)
			  {
			const CU::Vector4f& positionfirst = aMeshRenderFirst->gameObject->transform->GetPosition();
			const CU::Vector4f& positionSecond = aMeshRendererSecond->gameObject->transform->GetPosition();

			const CU::Vector4f& ToCameraFirst = positionfirst - cameraPosition;
			const CU::Vector4f& ToCameraSecond = positionSecond - cameraPosition;

			const float distanceFirstSquared = ToCameraFirst.LengthSqrNoW();
			const float distanceSecondSquared = ToCameraSecond.LengthSqrNoW();

			return distanceFirstSquared > distanceSecondSquared; });

	std::sort(myDefferedObjects.begin(), myDefferedObjects.end(), [&cameraPosition](MeshRenderer *aMeshRenderFirst, MeshRenderer *aMeshRendererSecond)
			  {
			const CU::Vector4f& positionfirst = aMeshRenderFirst->gameObject->transform->GetPosition();
			const CU::Vector4f& positionSecond = aMeshRendererSecond->gameObject->transform->GetPosition();

			const CU::Vector4f& ToCameraFirst = positionfirst - cameraPosition;
			const CU::Vector4f& ToCameraSecond = positionSecond - cameraPosition;

			const float distanceFirstSquared = ToCameraFirst.LengthSqrNoW();
			const float distanceSecondSquared = ToCameraSecond.LengthSqrNoW();

			return distanceFirstSquared < distanceSecondSquared; });

	/*std::sort(mySpriteObjects.begin(), mySpriteObjects.end(), [&cameraPosition](Sprite2DRenderer* aSpriteRenderer0, Sprite2DRenderer* aSpriteRenderer1)
		{
			return aSpriteRenderer0->GetLayer() < aSpriteRenderer1->GetLayer();
		});*/
}

void GraphicsEngine::RenderShadows()
{
	myCommandList.Enqueue([]()
						  {
		DirectionalLight::Get()->BindLightFrameBuffer();
		glCullFace(GL_BACK); });

	Render<RenderShadowCommand>();

	myCommandList.Enqueue([&]()
						  {
			glCullFace(GL_FRONT);
			CU::GameCamera::main->Use();

			const CU::GameCamera& dirCamera = DirectionalLight::Get()->GetCamera();

			const CU::Matrix4x4<float>& projectionMatrix = dirCamera.GetProjectionMatrix();
			const CU::Matrix4x4<float>& viewMatrix = dirCamera.GetInverse();

			Material::SetGlobalMatrix4x4("lightSpaceMatrix", viewMatrix * projectionMatrix);

			const CU::Vector2<unsigned>& resolution = GraphicsEngine::Get().GetResolution();
			glViewport(0, 0, resolution.x, resolution.y);
			glBindFramebuffer(GL_FRAMEBUFFER, previousBoundFramBuffer);

			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, DirectionalLight::Get()->GetShadowMapTexture()); });
}

GraphicsEngine::GraphicsEngine()
{
}

void GraphicsEngine::RenderSprites(bool isPixelpick)
{
	if (isPixelpick)
	{
		for (auto &object : mySpriteObjects)
			myCommandList.Enqueue<RenderInstancedSpriteCommand>(&object.second, &myPixelPickingMaterial, false);

		return;
	}

	for (auto &object : mySpriteObjects)
		myCommandList.Enqueue<RenderInstancedSpriteCommand>(&object.second, nullptr, true);
}

void GraphicsEngine::RenderPixelpicking(bool forced)
{

	myCommandList.Enqueue([&]
						  {
			glBindFramebuffer(GL_FRAMEBUFFER, myPixelPickingRenderTarget);

			glViewport(0, 0, 1920, 1080);
			glClearColor(0, 0, 0, 0);
			glClear(GL_COLOR_BUFFER_BIT); });

	RenderSprites(true);

	myCommandList.Enqueue([&]
						  {
			//ImGui::GetForegroundDrawList()->AddCircle(ImVec2((float)pos.x, (float)pos.y), 4, IM_COL32(255, 255, 0, 255));

			bool mouseCurrent = (glfwGetMouseButton(MainSingleton::Get().myWindow, 0) == GLFW_PRESS);

			if (forced || (mouseCurrent && !mousePrev))
			{
				CU::Vector2d mousePos;
				glfwGetCursorPos(MainSingleton::Get().myWindow, &mousePos.x, &mousePos.y);
			
				CU::Vector2i windowPos;
				glfwGetWindowPos(MainSingleton::Get().myWindow, &windowPos.x, &windowPos.y);
			
				CU::Vector2i windowSize;
				glfwGetWindowSize(MainSingleton::Get().myWindow, &windowSize.x, &windowSize.y);

				CU::Vector2i min{ windowPos.x,  windowPos.y };
				CU::Vector2i max{ windowPos.x + windowSize.x, windowPos.y + windowSize.y };

				auto pos = Editor::Get().GetPositionSceneRelative(
					CU::Vector2i(min.x, min.y),
					CU::Vector2i(max.x, max.y),
					CU::Vector2i(mousePos.x, mousePos.y));

				auto val = ReadPixel(CU::Vector2i(pos.x, 1080 - pos.y));

				int pickedID = val.x + val.y * 256 + val.z * 256 * 256;

				auto& gameObjects = MainSingleton::Get().myGameObjects;

				for (auto& [_, gameobject] : gameObjects)
				{
					if (gameobject->GetRawID() == pickedID)
					{
						if (InputHandler::GetKey(keycode::SHIFT))
							MainSingleton::Get().mySelectedObjects.emplace_back(gameobject);
						else
							MainSingleton::Get().mySelectedObjects = { gameobject };
					}
				}
			}

			glViewport(0, 0, myResolution.x, myResolution.y);
			glBindFramebuffer(GL_FRAMEBUFFER, previousBoundFramBuffer); 

			mousePrev = mouseCurrent; });
}

float GraphicsEngine::GetFPS()
{
	return fps;
}

float GraphicsEngine::GetAverageFPS()
{
	return fpsAvrage;
}

void GraphicsEngine::RenderOutline()
{
	if (MainSingleton::Get().mySelectedObjects.empty())
		return;
	if (!MainSingleton::Get().mySelectedObjects.front())
		return;

	myCommandList.Enqueue([&]
						  {
			glBindFramebuffer(GL_FRAMEBUFFER, myOuline2DRenderTarget);

			glViewport(0, 0, 1920, 1080);
			glClearColor(0, 0, 0, 0);
			glClear(GL_COLOR_BUFFER_BIT); });

	for (const auto &selectedObject : MainSingleton::Get().mySelectedObjects)
	{
		auto selectedSprite = selectedObject->GetComponent<Sprite2DRenderer>();

		myCommandList.Enqueue([&, selectedSprite]
							  {
				selectedSprite->GetMaterial().Use(false, true);
				const CU::Vector3f& pixelpickColor(selectedSprite->gameObject->GetPixelPickRGB());
				myOuline2DMaterial.SetVector3("pixelPickingID", pixelpickColor); });

		myCommandList.Enqueue<RenderSpriteCommand>(selectedSprite->gameObject, &myOuline2DMaterial, false);
	}

	myCommandList.Enqueue([&]
						  {
			glViewport(0, 0, myResolution.x, myResolution.y);
			glBindFramebuffer(GL_FRAMEBUFFER, previousBoundFramBuffer); });

	myCommandList.Enqueue([&]
						  {
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			myFullscreenPixelpickMaterial.Use(true, false);

			myFullscreenPixelpickMaterial.BindTexture(DIFFUSE, myOuline2DTexture, "pixelPickTexture");

			//CU::Matrix4x4<float> objectMat;

			myFullscreenPixelpickMaterial.SetVector4("material.albedo", myFullscreenPixelpickMaterial.GetAlbedo());

			mySprite.DrawSprite(); });
}

CU::Vector4u GraphicsEngine::ReadPixel(const CU::Vector2i &aPos)
{
	glFlush();
	glFinish();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	unsigned char data[4];
	glReadPixels(aPos.x, aPos.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

	return CU::Vector4u(data[0], data[1], data[2], data[3]);
}

void ImGuiText(const char *aText)
{
#ifdef NONWIN32
	ImGui::Text(aText);
#elif NONUNIX
	ImGui::Text("%s", aText);
#endif
}

template <class T>
void GraphicsEngine::Render()
{
	for (auto &object : myForwardObjects)
	{
		if (object->myLayer == 0)
		{
			myCommandList.Enqueue<T>(object->gameObject, object->GetMeshAdress(), &object->GetMaterial());
		}
	}

	for (auto &object : myForwardObjects)
	{
		if (object->myLayer == 0)
		{
			continue;
		}

		myCommandList.Enqueue([]()
							  {
			glStencilFunc(GL_ALWAYS, 2, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); });

		if (myForwardObjectsToRemove.find(object->myLayer) == myForwardObjectsToRemove.end())
		{
			std::vector<MeshRenderer *> &removeObject = myForwardObjectsToRemove.at(object->myLayer);
			for (MeshRenderer *meshrend : removeObject)
			{
				myCommandList.Enqueue<T>(meshrend->gameObject, meshrend->GetMeshAdress(), &meshrend->GetMaterial());
			}
		}

		myCommandList.Enqueue([]()
							  {
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); });

		myCommandList.Enqueue<T>(object->gameObject, object->GetMeshAdress(), &object->GetMaterial());
	}

	for (auto &object : myDefferedObjects)
		myCommandList.Enqueue<T>(object->gameObject, object->GetMeshAdress(), &object->GetMaterial());

	// Enqueue sprite for defferedRendering
}

void GraphicsEngine::LobbyDisplay()
{
#ifdef USENET
	ImGui::Begin("Lobbies avaible");

	for (int lobbiesAvaible : MainSingleton::Get().myNetworkManager->myLobbiesAvaible)
	{
		std::stringstream lobbyStream;
		lobbyStream << "Join Lobby: " << lobbiesAvaible;

		if (ImGui::Button(lobbyStream.str().c_str()))
			MainSingleton::Get().myNetworkManager->GetNetRole()->SendData(&lobbiesAvaible, sizeof(lobbiesAvaible), DataTypeSent::JOINLOBBY, 0, 0, true);
	}

	ImGui::End();
#endif
}

void GraphicsEngine::FPSDisplay()
{
	static float deltaTime = 0;
	static float frames = 0;
	const float updateEvery = 0.5f;
	static float totalTime = updateEvery;

	// Information display
	{
		ImGui::SetNextWindowPos(ImVec2(0, 50));
		ImGui::SetNextWindowSize(ImVec2(500, 500));
		ImGui::Begin("Information", (bool *)1, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);

		// FPS
		{
			deltaTime = CommonUtilities::Timer::GetDeltaTime();
			fps = 1 / deltaTime;
			fpsAvrage += fps;

			informationStream.str("");

			// Avrage FPS
			{
				informationStream << "Avrage FPS: " << fpsAvrage / ++frames << std::endl;
			}

			// Current FPS
			{
				if (totalTime >= updateEvery)
				{
					keptFPS = fps;
					totalTime = 0;
				}
				else
				{
					totalTime += deltaTime;
				}

				informationStream << "FPS: " << keptFPS << std::endl;
			}
		}

		// Network information
		{
#ifdef USENET
			float ping = MainSingleton::Get().myNetworkManager->GetNetRole()->GetPing();
			informationStream << "Ping: " << ping << "ms" << std::endl;

			int packetloss = MainSingleton::Get().myNetworkManager->GetNetRole()->GetPacketLoss();
			informationStream << "PacketLoss: " << packetloss << "%" << std::endl;

			int sentPS = MainSingleton::Get().myNetworkManager->GetNetRole()->GetSentPerSecond();
			informationStream << "Sent: " << sentPS << "/s" << std::endl;

			int recievedPS = MainSingleton::Get().myNetworkManager->GetNetRole()->GetRecievedPerSecond();
			informationStream << "Recieved: " << recievedPS << "/s" << std::endl;
#endif

			ImGuiText(informationStream.str().c_str());
		}

		ImGui::End();
	}
}

void GraphicsEngine::Render()
{
	glClearColor(0.2f, 0.3f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glGetIntegerv(GL_FRAMEBUFFER_BINDING, (int *)&previousBoundFramBuffer);

	// RenderSorting();

	RenderPixelpicking();
	RenderOutline();

	// RenderShadows();
	// Render<RenderMeshCommand>();

	RenderSprites();

	myCommandList.Execute();
	myCommandList.Reset();

	/*ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();*/
}

void GraphicsEngine::ImGuiBegin()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void GraphicsEngine::ImGuiRender()
{
	ImGuiViewport *viewport;

	viewport = ImGui::GetMainViewport();

	// Submit a window filling the entire viewport
	// ImGui::SetNextWindowPos(ImVec2(-8, -8));
	// ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x / 2 + 10, viewport->WorkSize.y + 10));
	// ImGui::SetNextWindowViewport(viewport->ID);

	ImGuiWindowFlags host_window_flags = 0;
	host_window_flags |= ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs;

	ImGuiDockNodeFlags dock_flags = 0;
	// dock_flags |= ImGuiDockNodeFlags_PassthruCentralNode;

	auto dockspace_id = ImGui::GetID("DockSpace");
	ImGui::DockSpaceOverViewport(dockspace_id, NULL, dock_flags);

	FPSDisplay();
#ifdef USENET
	LobbyDisplay();
#endif
}

void GraphicsEngine::ImGuiEnd()
{
	ImGui::Render();
}

void GraphicsEngine::SetResolution(const CU::Vector2<unsigned> &aResolution)
{
	myResolution = aResolution;
}
const CU::Vector2<unsigned> &GraphicsEngine::GetResolution()
{
	return myResolution;
}