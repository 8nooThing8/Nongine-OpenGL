#include "GameWorld.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <ImGui/imgui_impl_win32.h>
#include <ImGui/imgui_impl_dx11.h>

#include <rapidjson-master/include/rapidjson/filereadstream.h>
#include <rapidjson-master/include/rapidjson/document.h>

#include "InputHandler.h"
#include "../Engine/GameEngine/MainSingleton.h"

#include <iterator>

#include "Timer.h"

#include "../Engine/GameEngine/Components/GameObject.h"

#include "../Engine/GraphicsEngine/ShaderCompiler.h"

#include "../Engine/GameEngine/Components/CameraComponent.h"
#include "../Engine/GameEngine/GameCamera.h"
#include "../Engine/GameEngine/Components/MeshRenderer.h"
#include "../Engine/GameEngine/Components/SpriteRenderer.h"
#include "../Engine/GameEngine/Components/Move.hpp"
#include "../Engine/GameEngine/Components/Spin.hpp"
#include "../Engine/GameEngine/Components/Particles/ParticleSystem.h"
#include "../Engine/GameEngine/Components/Trail/TrailSystem.h"
#include "../Engine/GameEngine/Components/InstanceComponent.h"
#include "../Engine/GameEngine/ModelSprite/Sprite.h"
#include "../Engine/GameEngine/Components/DirectionalLight.h"
#include "../Engine/GameEngine/Components/ReflectionProbe.h"
#include "../Engine/GameEngine/Components/Networking/NetworkTransform.h"
#include "../ImGuizmo\ImGuizmo.h"

#include "../Engine/GameEngine/Components/Physics/Colliders/BoxCollider.h"
#include "../Engine/GameEngine/Components/Physics/Colliders/CapsuleCollider.h"
#include "../Engine/GameEngine/Components/Physics/Colliders/MeshCollider.h"
#include "../Engine/GameEngine/Components/Physics/CharacterController.h"

#include "../Engine/GameEngine/NetworkManager.h"

#include "../Engine/GameEngine/ComponentManager.h"

#include "../Engine/GameEngine/SpriteEditor.h"
#include "../Engine/GameEngine/Components/Animator.h"
#include "../Engine/GameEngine/Components/Transform.h"
#include "../Engine/GameEngine/ModelSprite/ModelFactory.h"

#include "../Engine/GameEngine/SceneManagerInternal.h"

#include "../PhysicsEngine/PhysicsEngine.h"

#include <pix3.h>

using namespace std;


GameWorld::GameWorld(HWND aHWND)
{
	inspectorCamera = new InspectorCamera();
	inspectorCamera->position = CommonUtilities::Vector4<float>(0, 100, -100, 1);

	myAssetsTab = AssetsTab();


	myDeffRendering.Init(aHWND);

	MainSingleton::Get()->MainSingleton::Get()->activeScene = new Scene("Empty Scene");
}

void GameWorld::ChildChecker(std::vector<std::shared_ptr<GameObject>>& aChildren, const int aTotalIndent)
{
	for (int i = 0; i < aChildren.size(); i++)
	{
		ImGui::PushID(aChildren[i]->myID);

		ImGui::Dummy(ImVec2(static_cast<float>(aTotalIndent), 0));

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));

		if (aChildren[i]->open)
		{
			if (ImGui::ArrowButton(" ", ImGuiDir_Up))
			{
				aChildren[i]->open = false;
			}
		}
		else
		{
			if (ImGui::ArrowButton(" ", ImGuiDir_Down))
			{
				aChildren[i]->open = true;
			}
		}

		ImGui::PopStyleColor();

		ImGui::SameLine();

		if (MainSingleton::Get()->mySelectedObject == aChildren[i])
			ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 255, 255, 50));
		else
			ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 255, 255, 120));

		if (ImGui::Button(aChildren[i]->GetName().c_str()))
		{
			MainSingleton::Get()->mySelectedObject = aChildren[i];
		}

		std::vector<std::shared_ptr<GameObject>> children = aChildren[i]->GetChildren();

		ImGui::PopStyleColor();

		ImGui::PopID();

		ChildChecker(children, aTotalIndent + 19);
	}
}

void GameWorld::EnterPlayMode()
{
	MainSingleton::Get()->myIsPlaying = true;

	MainSingleton::Get()->myComponentManager->StartComponents();
	MainSingleton::Get()->myNetworkManager->StartGame();

	gameCamera->SetActiveCameraAsThis();

	//SceneManagerInternal::SaveScene(MainSingleton::Get()->activeScene, "Assets/Scenes/Play.no");
}
void GameWorld::ExitPlayMode()
{
	MainSingleton::Get()->myIsPlaying = false;
	MainSingleton::Get()->activeScene = SceneManagerInternal::LoadScene("Assets/Scenes/Play.no");

	MainSingleton::Get()->myComponentManager->StopComponents();

	inspectorCamera->SetActiveCameraAsThis();
}

bool ButtonCenteredOnLine(const char* label, const float alignment = 0.5f)
{
	ImGuiStyle& style = ImGui::GetStyle();

	float size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
	float avail = ImGui::GetContentRegionAvail().x;

	float off = (avail - size) * alignment;
	if (off > 0.0f)
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

	return ImGui::Button(label);
}

#pragma region TestWithSampleMaterials
static CommonUtilities::Vector4<float> LoadColor(std::string aFilePath)
{
	CommonUtilities::Vector4<float> returnColor;

	FILE* fp;
	char readBuffer[8196];

	fopen_s(&fp, aFilePath.c_str(), "r");

	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

	rapidjson::Document d;
	d.ParseStream(is);

	fclose(fp);

	if (!d.IsObject())
	{
		throw std::runtime_error("Invalid JSON format: root should be an object.");
	}

	returnColor.x = d["Color"]["r"].GetFloat() * (1.f / 255.f);
	returnColor.y = d["Color"]["g"].GetFloat() * (1.f / 255.f);
	returnColor.z = d["Color"]["b"].GetFloat() * (1.f / 255.f);
	returnColor.w = d["Color"]["a"].GetFloat() * (1.f / 255.f);

	return returnColor;
}

static std::array<std::wstring, 3> LoadTexture(std::string aFilePath)
{
	std::array<std::wstring, 3> returnDiffuseNormal;

	FILE* fp;
	char readBuffer[8196];

	fopen_s(&fp, aFilePath.c_str(), "r");

	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

	rapidjson::Document d;
	d.ParseStream(is);

	fclose(fp);

	if (!d.IsObject())
	{
		throw std::runtime_error("Invalid JSON format: root should be an object.");
	}

	std::string textureDiffuse = d["Texture"]["diffuse"].GetString();
	std::string textureNormal = d["Texture"]["normal"].GetString();
	std::string textureMaterial = d["Texture"]["material"].GetString();

	returnDiffuseNormal[0] = std::wstring(textureDiffuse.begin(), textureDiffuse.end());
	returnDiffuseNormal[1] = std::wstring(textureNormal.begin(), textureNormal.end());
	returnDiffuseNormal[2] = std::wstring(textureMaterial.begin(), textureMaterial.end());

	return returnDiffuseNormal;
}
#pragma endregion

void GameWorld::SampleSceneCreation()
{
	// Directional light
	{
		MainSingleton::Get()->mySelectedObject = NewEmptyGameObject("Directional light");

		MainSingleton::Get()->mySelectedObject->transform->position = CommonUtilities::Vector4<float>(0.f, 100.f, -115.f, 1.f);
		MainSingleton::Get()->mySelectedObject->transform->rotation = CommonUtilities::Vector4<float>(CommonUtilities::DegToRad(45.f), CommonUtilities::DegToRad(-45.f), 0.f, 1.f);

		MainSingleton::Get()->mySelectedObject->AddComponent<DirectionalLight>();
		MeshRenderer* meshRenderer = MainSingleton::Get()->mySelectedObject->AddComponent<MeshRenderer>();

		meshRenderer->GetMaterial().SetShader(L"Assets/Shaders/UnLit_PS.hlsl", L"Assets/Shaders/Default_VS.hlsl");

		meshRenderer->myMesh = CreateModel("Assets/Models/IcoSphere.fbx");

		meshRenderer->SetCastShadow(false);
	}

	// Chest instanced
	//{
	//	std::string chest = "Assets/Models/SM_Chest.fbx";

	//	MainSingleton::Get()->mySelectedObject = NewGameObject("Particle_Chest", chest);

	//	MainSingleton::Get()->mySelectedObject->transform->position = CommonUtilities::Vector4<float>(77.5f, 75.f, 219.2f, 1.f);
	//	MainSingleton::Get()->mySelectedObject->transform->rotation = CommonUtilities::Vector4<float>(0.f, -38.9f, 0.f, 1.f);

	//	std::array<std::wstring, 3> textures = LoadTexture("Assets/MAterials/ChestMaterial.json");

	//	MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetColor(LoadColor("Assets/MAterials/ChestMaterial.json"));
	//	MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetShader(L"Assets/Shaders/Deffered_PS.hlsl", L"Assets/Shaders/Default_VS.hlsl", textures[0], textures[1]);

	//	MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetMetallicTexture(textures[2]);
	//	MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetRoughnessTexture(textures[2]);
	//	MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetAOTexture(textures[2]);

	//	MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetBloomStrength(2.650f);

	//	MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetAOStrength(2);

	//	MainSingleton::Get()->mySelectedObject->AddComponent<Spin>();
	//}

	// Chest 2
	{
		std::string chest = "Assets/Models/SM_Chest.fbx";

		MainSingleton::Get()->mySelectedObject = NewGameObject("Particle_Chestssss", chest);

		MainSingleton::Get()->mySelectedObject->transform->position = CommonUtilities::Vector4<float>(-97.f, -45.f, 301.f, 1.f);
		MainSingleton::Get()->mySelectedObject->transform->rotation = CommonUtilities::Vector4<float>(0.f, 0.f, 0.f, 1.f);

		std::array<std::wstring, 3> textures = LoadTexture("Assets/MAterials/ChestMaterial.json");

		auto meshRenderer = MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>();

		meshRenderer->GetMaterial().SetColor(LoadColor("Assets/MAterials/ChestMaterial.json"));
		meshRenderer->GetMaterial().SetShader(L"Assets/Shaders/Deffered_PS.hlsl", L"Assets/Shaders/Default_VS.hlsl", textures[0], textures[1]);

		meshRenderer->GetMaterial().SetMetallicTexture(textures[2]);
		meshRenderer->GetMaterial().SetRoughnessTexture(textures[2]);
		meshRenderer->GetMaterial().SetAOTexture(textures[2]);

		meshRenderer->GetMaterial().SetAOStrength(2);

		auto collider = MainSingleton::Get()->mySelectedObject->AddComponent<MeshCollider>();
		collider->SetMesh(meshRenderer->myMesh);

		MainSingleton::Get()->mySelectedObject->AddComponent<Spin>();

		//auto maxBounds = meshRenderer->myMesh->BoxBounds.Max;
		//auto minBounds = meshRenderer->myMesh->BoxBounds.Min;

		//CommonUtilities::Vector3f maxBound(maxBounds[0], maxBounds[1], maxBounds[2]);
		//CommonUtilities::Vector3f minBound(minBounds[0], minBounds[1], minBounds[2]);

		//collider->SetFullExtents(maxBound - minBound);
	}

	// Chest 3
	{
		std::string chest = "Assets/Models/SM_Chest.fbx";

		MainSingleton::Get()->mySelectedObject = NewGameObject("Particle_Chest", chest);

		MainSingleton::Get()->mySelectedObject->transform->position = CommonUtilities::Vector4<float>(377.5f, 10.f, 300.f, 1.f);
		MainSingleton::Get()->mySelectedObject->transform->rotation = CommonUtilities::Vector4<float>(0.f, 0.f, 0.f, 1.f);

		std::array<std::wstring, 3> textures = LoadTexture("Assets/MAterials/ChestMaterial.json");

		auto meshRenderer = MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>();

		meshRenderer->GetMaterial().SetColor(LoadColor("Assets/MAterials/ChestMaterial.json"));
		meshRenderer->GetMaterial().SetShader(L"Assets/Shaders/Deffered_PS.hlsl", L"Assets/Shaders/Default_VS.hlsl", textures[0], textures[1]);

		meshRenderer->GetMaterial().SetMetallicTexture(textures[2]);
		meshRenderer->GetMaterial().SetRoughnessTexture(textures[2]);
		meshRenderer->GetMaterial().SetAOTexture(textures[2]);

		meshRenderer->GetMaterial().SetAOStrength(2);

		auto collider = MainSingleton::Get()->mySelectedObject->AddComponent<MeshCollider>();

		collider->SetMesh(meshRenderer->myMesh);

		

		//auto maxBounds = meshRenderer->myMesh->BoxBounds.Max;
		//auto minBounds = meshRenderer->myMesh->BoxBounds.Min;

		//CommonUtilities::Vector3f maxBound(maxBounds[0], maxBounds[1], maxBounds[2]);
		//CommonUtilities::Vector3f minBound(minBounds[0], minBounds[1], minBounds[2]);

		//collider->SetFullExtents(maxBound - minBound);
	}

	//Tga bro
	{
		std::string tgaBro = "Assets/Models/SK_C_TGA_Bro.fbx";

		MainSingleton::Get()->mySelectedObject = NewGameObject("TgaBro-Network", tgaBro);

		MainSingleton::Get()->mySelectedObject->transform->position = CommonUtilities::Vector4<float>(0, 0, 0, 1);
		MainSingleton::Get()->mySelectedObject->transform->rotation = CommonUtilities::Vector4<float>(0, 0, 0, 1);


		NetworkTransform* networkTransform = MainSingleton::Get()->mySelectedObject->AddComponent<NetworkTransform>();
		networkTransform->sendData = false;

		Animator* animator = MainSingleton::Get()->mySelectedObject->AddComponent<Animator>();

		std::string animationToBeSet = "Assets/Animations/A_C_TGA_Bro_Walk.fbx";

		animator->SetAnimation(animationToBeSet);

		animator->SetSecondAnimation("Assets/Animations/A_C_TGA_Bro_Run.fbx");

		animator->AddLayeredAnimation("RightShoulder");
		animator->SetLayeredAnimation("Assets/Animations/A_C_TGA_Bro_Idle_Wave.fbx", 0);

		std::array<std::wstring, 3> textures = LoadTexture("Assets/MAterials/TGABroMaterial.json");

		MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetColor(CommonUtilities::Vector4f(0.15f, 0.5f, 0.15f, 1.f));
		MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetShader(L"Assets/Shaders/Deffered_PS.hlsl", L"Assets/Shaders/Default_VS.hlsl", textures[0], textures[1]);

		MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetMetallicTexture(textures[2]);
		MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetRoughnessTexture(textures[2]);
		MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetAOTexture(textures[2]);

		MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetBloomStrength(2.5f);

		auto capCollider = MainSingleton::Get()->mySelectedObject->AddComponent<CapsuleCollider>();

		capCollider->SetRadius(40);
		capCollider->SetHalfHeight(40);
		capCollider->SetPositionOffset(CommonUtilities::Vector3f(80, 0, 0));
	}

	//Tga bro
	{
		std::string tgaBro = "Assets/Models/SK_C_TGA_Bro.fbx";

		MainSingleton::Get()->mySelectedObject = NewGameObject("TgaBro", tgaBro);

		MainSingleton::Get()->mySelectedObject->transform->position = CommonUtilities::Vector4<float>(0, 200, 0, 1.f);
		MainSingleton::Get()->mySelectedObject->transform->rotation = CommonUtilities::Vector4<float>(0.f, 0.f, 0.f, 1.f);


		NetworkTransform* networkTransform = MainSingleton::Get()->mySelectedObject->AddComponent<NetworkTransform>();
		networkTransform->sendData = true;

		Animator* animator = MainSingleton::Get()->mySelectedObject->AddComponent<Animator>();

		std::string animationToBeSet = "Assets/Animations/A_C_TGA_Bro_Walk.fbx";

		animator->SetAnimation(animationToBeSet);

		animator->SetSecondAnimation("Assets/Animations/A_C_TGA_Bro_Run.fbx");

		animator->AddLayeredAnimation("RightShoulder");
		animator->SetLayeredAnimation("Assets/Animations/A_C_TGA_Bro_Idle_Wave.fbx", 0);

		std::array<std::wstring, 3> textures = LoadTexture("Assets/MAterials/TGABroMaterial.json");

		MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetColor(LoadColor("Assets/MAterials/TGABroMaterial.json"));
		MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetShader(L"Assets/Shaders/Deffered_PS.hlsl", L"Assets/Shaders/Default_VS.hlsl", textures[0], textures[1]);

		MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetMetallicTexture(textures[2]);
		MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetRoughnessTexture(textures[2]);
		MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetAOTexture(textures[2]);

		MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetBloomStrength(2.5f);

		//MainSingleton::Get()->mySelectedObject->AddComponent<RigidBody>();

		//auto boxCollider = MainSingleton::Get()->mySelectedObject->AddComponent<BoxCollider>();

		//boxCollider->SetCollisionLayer(CollisionLayer::Default);
		//boxCollider->SetHalfExtents(CommonUtilities::Vector3f(15, 50, 15));
		//boxCollider->SetPositionOffset(CommonUtilities::Vector3f(0, 50, 0));


		auto characterController = MainSingleton::Get()->mySelectedObject->AddComponent<CharacterController>();
		characterController->SetRadius(40);
		characterController->SetHalfHeigt(50);

		characterController->SetCollisionLayer(CollisionLayer::Default);

		MainSingleton::Get()->mySelectedObject->AddComponent<Move>();
	}

	// GameCamera
	{
		auto go = NewEmptyGameObject("Camera");

		go->transform->position = CommonUtilities::Vector4<float>(0, 151, -100, 1);
		go->transform->rotation = CommonUtilities::Vector4<float>(-20, 0, 0, 1);
		go->transform->scale = CommonUtilities::Vector4<float>(1, 1, 1, 1);

		gameCamera = go->AddComponent<CameraComponent>();
		gameCamera->myGameObjectToFolow = &(*MainSingleton::Get()->mySelectedObject);
	}

	// Platform
	{
		std::string Cube = "Assets/Models/Plane.fbx";
		MainSingleton::Get()->mySelectedObject = NewGameObject("Cube", Cube);

		MainSingleton::Get()->mySelectedObject->transform->position = CommonUtilities::Vector4<float>(0, -1, 0, 1);
		MainSingleton::Get()->mySelectedObject->transform->scale = CommonUtilities::Vector4<float>(10000, 1, 10000, 1);

		MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetShader(L"Assets/Shaders/Deffered_PS.hlsl", L"Assets/Shaders/Default_VS.hlsl");

		MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetColor(CommonUtilities::Vector4<float>(0.3f, 0.3f, 0.3f, 1.f));

		MainSingleton::Get()->mySelectedObject->AddComponent<BoxCollider>();
	}

	//// Reflection probe
	//{
	//	std::string IcoSphere = "Assets/Models/IcoSphere.fbx";

	//	MainSingleton::Get()->mySelectedObject = NewGameObject("IcoSphere", IcoSphere);

	//	MainSingleton::Get()->mySelectedObject->transform->position = CommonUtilities::Vector4<float>(-100.f, 75.f, -100.f, 1.f);
	//	MainSingleton::Get()->mySelectedObject->transform->rotation = CommonUtilities::Vector4<float>(0.f, 0, 0.f, 1.f);
	//	MainSingleton::Get()->mySelectedObject->transform->scale = CommonUtilities::Vector4<float>(30.f, 30.f, 30.f, 1.f);

	//	MainSingleton::Get()->mySelectedObject->AddComponent<ReflectionProbe>();

	//	MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->myDeffered = false;
	//}

	//// Reflection probe
	//{
	//	std::string IcoSphere = "Assets/Models/IcoSphere.fbx";

	//	MainSingleton::Get()->mySelectedObject = NewGameObject("IcoSphere", IcoSphere);

	//	MainSingleton::Get()->mySelectedObject->transform->position = CommonUtilities::Vector4<float>(-200.f, 75.f, -100.f, 1.f);
	//	MainSingleton::Get()->mySelectedObject->transform->rotation = CommonUtilities::Vector4<float>(0.f, 0, 0.f, 1.f);
	//	MainSingleton::Get()->mySelectedObject->transform->scale = CommonUtilities::Vector4<float>(30.f, 30.f, 30.f, 1.f);

	//	MainSingleton::Get()->mySelectedObject->AddComponent<ReflectionProbe>();

	//	MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->myDeffered = false;
	//}

	// Color checker
	{
		std::string IcoSphere = "Assets/Models/SM_Color_Checker.fbx";

		MainSingleton::Get()->mySelectedObject = NewGameObject("TestCube", IcoSphere);

		MainSingleton::Get()->mySelectedObject->transform->position = CommonUtilities::Vector4<float>(-150.f, 75.f, -180.f, 1.f);
		MainSingleton::Get()->mySelectedObject->transform->rotation = CommonUtilities::Vector4<float>(-90.f * deg2rad, 0, 0.f, 1.f);
		MainSingleton::Get()->mySelectedObject->transform->scale = CommonUtilities::Vector4<float>(1.f, 1.f, 1.f, 1.f);

		std::array<std::wstring, 3> textures = LoadTexture("Assets/MAterials/TestCubeMaterials.json");

		MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetColor(LoadColor("Assets/MAterials/TestCubeMaterials.json"));
		MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetShader(L"Assets/Shaders/Deffered_PS.hlsl", L"Assets/Shaders/Default_VS.hlsl", textures[0], textures[1]);

		MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetMetallicTexture(textures[2]);
		MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetRoughnessTexture(textures[2]);
		MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetAOTexture(textures[2]);

		MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetBloomStrength(2.650f);

		auto collider = MainSingleton::Get()->mySelectedObject->AddComponent<BoxCollider>();

		auto maxBounds = MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->myMesh->BoxBounds.Max;
		auto minBounds = MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->myMesh->BoxBounds.Min;

		CommonUtilities::Vector3f maxBound(maxBounds[0], maxBounds[1], maxBounds[2]);
		CommonUtilities::Vector3f minBound(minBounds[0], minBounds[1], minBounds[2]);

		collider->SetFullExtents(maxBound - minBound);
	}

	// Skybox
	{
		std::string skybox = "Assets/Models/Skybox.fbx";

		std::shared_ptr<GameObject> GO = std::make_shared<GameObject>("Skybox");
		GO->transform->position = CommonUtilities::Vector4<float>(0.f, 0.f, 0.f, 1.f);

		MeshRenderer* mr = GO->AddComponent<MeshRenderer>(false);

		mr->myMesh = CreateModel(skybox);

		GO->transform->position = CommonUtilities::Vector4<float>(0.f, 0.f, 0.f, 1.f);
		GO->transform->rotation = CommonUtilities::Vector4<float>(0.f, 0, 0.f, 1.f);
		GO->transform->scale = CommonUtilities::Vector4<float>(-1000.f, -1000, -1000.f, 1.f);

		GO->GetComponent<MeshRenderer>()->GetMaterial().SetShader(L"Assets/Shaders/Skybox_PS.hlsl", L"Assets/Shaders/SkyBox_VS.hlsl");

		MainSingleton::Get()->mySkyBox = GO;
		MainSingleton::Get()->mySkyBoxMeshRend = GO->GetComponent<MeshRenderer>();
	}

	// Example sprite
	{
		std::string sprite = "Folder";

		MainSingleton::Get()->mySelectedObject = NewEmptyGameObject("Sprite");

		MainSingleton::Get()->mySelectedObject->AddComponent<SpriteRenderer>();

		std::filesystem::path path = std::filesystem::path("Assets/Sprites/Folder.dds");

		MainSingleton::Get()->mySelectedObject->GetComponent<SpriteRenderer>()->GetSprite() = new Sprite(path);

		MainSingleton::Get()->mySelectedObject->transform->position = CommonUtilities::Vector4<float>(750.f, 355.f, 0, 1);
		MainSingleton::Get()->mySelectedObject->transform->scale = CommonUtilities::Vector4<float>(350.f, 350.f, 0, 1);
	}

	// Trail system
	{
		MainSingleton::Get()->mySelectedObject = NewEmptyGameObject("Trail system");

		MainSingleton::Get()->mySelectedObject->AddComponent<TrailSystem>();

		MainSingleton::Get()->mySelectedObject->transform->rotation = CommonUtilities::Vector4<float>(0, 0, CommonUtilities::deg2rad * -90, 0);
		MainSingleton::Get()->mySelectedObject->transform->position = CommonUtilities::Vector4<float>(200, 100, 0, 1);

		//MainSingleton::Get()->mySelectedObject->SetActive(false);

		MainSingleton::Get()->objectToAttach = &(*MainSingleton::Get()->mySelectedObject->transform);
	}

	// Instance renderer
	{
		MainSingleton::Get()->mySelectedObject = NewEmptyGameObject("Instance Render");

		InstanceComponent* insComp = MainSingleton::Get()->mySelectedObject->AddComponent<InstanceComponent>();

		MainSingleton::Get()->mySelectedObject->transform->rotation = CommonUtilities::Vector4<float>(0, 0, 0, 0);
		MainSingleton::Get()->mySelectedObject->transform->position = CommonUtilities::Vector4<float>(0, 0, 0, 1);

		insComp;

		//Animator* animator = MainSingleton::Get()->mySelectedObject->AddComponent<Animator>();

		//std::string animationToBeSet = "Assets/Animations/A_C_TGA_Bro_Walk.fbx";

		//insComp->Start();

		//animator->SetAnimation(animationToBeSet, insComp->instancedMesh);

		//animator->SetSecondAnimation("Assets/Animations/A_C_TGA_Bro_Run.fbx");

		//animator->AddLayeredAnimation("RightShoulder");
		//animator->SetLayeredAnimation("Assets/Animations/A_C_TGA_Bro_Idle_Wave.fbx", 0);
	}

	// Particle system
	{
		MainSingleton::Get()->mySelectedObject = NewEmptyGameObject("Particle system");

		MainSingleton::Get()->mySelectedObject->AddComponent<ParticleSystem>();

		MainSingleton::Get()->mySelectedObject->transform->rotation = CommonUtilities::Vector4<float>(0, 0, CommonUtilities::deg2rad * -90, 0);
		MainSingleton::Get()->mySelectedObject->transform->position = CommonUtilities::Vector4<float>(0, 100, 0, 1);


		//MainSingleton::Get()->mySelectedObject->SetActive(false);
	}

	//	MainSingleton::Get()->myComponentManager->StartComponents();
}

void GameWorld::RenderImGui(ImGuiIO& /*io*/)
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

std::string convertLPWSTRToString(const LPWSTR lpwstr)
{
	int length = WideCharToMultiByte(CP_UTF8, 0, lpwstr, -1, NULL, 0, NULL, NULL);
	char* buffer = new char[length];
	WideCharToMultiByte(CP_UTF8, 0, lpwstr, -1, buffer, length, NULL, NULL);
	std::string result(buffer);
	delete[] buffer;
	return result;
}

LPWSTR ConvertStringToLPWSTR(const std::string& str)
{
	int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	LPWSTR lpwstr = new wchar_t[size];
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, lpwstr, size);
	return lpwstr;
}

std::shared_ptr<GameObject> GameWorld::NewGameObject(const std::string& aName, std::string& aPath) const
{
	std::shared_ptr<GameObject> gameobject = std::make_shared<GameObject>(aName);
	gameobject->transform->position = CommonUtilities::Vector4<float>(0.f, 0.f, 0.f, 1.f);

	MeshRenderer* meshRenderer = gameobject->AddComponent<MeshRenderer>();

	Shaders::CompileShader(&meshRenderer->GetMaterial());

	meshRenderer->myDeffered = true;
	meshRenderer->myMesh = CreateModel(aPath);

	//gameobject->AddComponent<EngineInspectorElement>();

	MainSingleton::Get()->activeScene->gameObjects.emplace(aName, gameobject);

	return MainSingleton::Get()->activeScene->gameObjects[aName];
}

std::shared_ptr<GameObject> GameWorld::NewEmptyGameObject(const std::string& aName) const
{
	std::shared_ptr<GameObject> gameobject = std::make_shared<GameObject>(aName);
	gameobject->transform->position = CommonUtilities::Vector4<float>(0.f, 0.f, 0.f, 1.f);

	MainSingleton::Get()->activeScene->gameObjects.emplace(aName, gameobject);

	return gameobject;
}

void GameWorld::SceneRender()
{
	ImGui::Begin("Scene");

	if (InputHandler::GetInstance()->GetMouseButton(0) && !ImGuizmo::IsOver())
	{
		//MainSingleton::Get()->mySelectedObject = nullptr;
	}

	//ImGui::SetWindowSize(ImVec2(800, 450));

	ImVec2 windowSize = ImGui::GetWindowSize();
	float width = windowSize.x;
	float height = windowSize.y - 35;

	//if (MainSingleton::Get()->myIsPlaying)
	//{
	//	float width = 1920;
	//	float height = 1080;
	//}

	float windowPosX = ImGui::GetWindowPos().x;
	float windowPosY = ImGui::GetWindowPos().y;

	mousePos = { InputHandler::GetInstance()->GetRelativeMousePos().x, InputHandler::GetInstance()->GetRelativeMousePos().y };

	mousePos.x = static_cast<int>(CommonUtilities::MapValue<float>(0.f, 1920.f, ImGui::GetWindowPos().x, windowPosX + width, static_cast<float>(mousePos.x)));
	mousePos.y = static_cast<int>(CommonUtilities::MapValue<float>(0.f, 1080.f, ImGui::GetWindowPos().y, windowPosY + height, static_cast<float>(mousePos.y)));

	//std::cout << mousePos.x << "       " << mousePos.y << std::endl << std::endl;

	ImGui::Image((ImTextureID)GraphicsEngine::GetRHI()->mySceneSRV.Get(), ImVec2(width, height));

	GraphicsEngine::Get().myGameResolution = CommonUtilities::Vector2<float>(width, height);

	if (InputHandler::GetInstance()->GetMouseButtonDown(1) && ImGui::IsItemHovered())
	{
		InputHandler::GetInstance()->SetMousePositionNormalized(0.5f, 0.5f);
		InputHandler::GetInstance()->ShowMyCursor(false);
	}

	if (InputHandler::GetInstance()->GetMouseButton(1) && ImGui::IsItemHovered())
	{
		InputHandler::GetInstance()->ChangeMouseLockMode(InputHandler::Confined);
		inspectorCamera->Update(CommonUtilities::Time::GetDeltaTime());

	}
	else
	{
		InputHandler::GetInstance()->ChangeMouseLockMode(InputHandler::Unconfined);
		InputHandler::GetInstance()->ShowMyCursor(true);
	}

	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();

	ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());



	if (MainSingleton::Get()->mySelectedObject)
	{
		MainSingleton::Get()->mySelectedObject->transform->RenderImGUIZMO();
	}

	//ImGuizmo::DrawGrid(static_cast<float*>(transform), static_cast<float*>(Camera::main->GetProjectionMatrix()), static_cast<float*>(CommonUtilities::Matrix4x4<float>()), 1000.f);

	ImGui::End();
}

void GameWorld::RenderHierarchy()
{
	ImGui::Begin("Hierarchy");

	for (auto& obj : MainSingleton::Get()->activeScene->gameObjects)
	{
		ImGui::PushID(obj.second->myID);

		ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));

		if (obj.second->open)
		{
			if (ImGui::ArrowButton(" ", ImGuiDir_Up))
			{
				obj.second->open = false;
			}
		}
		else
		{
			if (ImGui::ArrowButton(" ", ImGuiDir_Down))
			{
				obj.second->open = true;
			}
		}

		ImGui::PopStyleColor();

		ImGui::SameLine();

		if (MainSingleton::Get()->mySelectedObject == obj.second)
			ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(50, 50, 50, 255));
		else
			ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(150, 150, 150, 255));

		if (ImGui::Button(obj.first.c_str()))
		{
			MainSingleton::Get()->mySelectedObject = obj.second;

			//MainSingleton::Get()->mySelectedObject->transform->UpdateImGuizmoMatrix();
		}

		static bool changeName = false;

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
		{
			obj.second->SetName("null");

			MainSingleton::Get()->mySelectedObject = nullptr;
		}

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("SELECTED", &obj.first, sizeof(int), ImGuiCond_Once);

			ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(150, 150, 150, 255));
			ImGui::Button(obj.first.c_str(), ImVec2(ImGui::CalcTextSize(obj.first.c_str()).x + 7, 0));
			ImGui::PopStyleColor();

			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SELECTED");

			if (payload)
			{
				assert(payload->DataSize == sizeof(int));

				std::string* numptr = reinterpret_cast<std::string*>(payload->Data);

				obj.second->PushNewChild(MainSingleton::Get()->activeScene->gameObjects[*numptr]);
				MainSingleton::Get()->activeScene->gameObjects[*numptr]->SetParent(obj.second.get());

				obj.second->open = true;

				MainSingleton::Get()->activeScene->gameObjects.erase(*numptr);
			}

			ImGui::EndDragDropTarget();
		}


		if (obj.second->open)
		{
			std::vector<std::shared_ptr<GameObject>> children = obj.second->GetChildren();

			int totalIndent = 19;

			ChildChecker(children, totalIndent);
		}

		ImGui::PopID();

		ImGui::PopStyleColor();
	}

	ImGui::End();
}

void GameWorld::RenderInspector()
{
	ImGui::Begin("Inspector");

	if (MainSingleton::Get()->mySelectedObject)
	{
		ImGui::Checkbox("##Enabled", &MainSingleton::Get()->mySelectedObject->GetActive());
		ImGui::SameLine();

		char nameBuffer[256];

		std::string nameString = MainSingleton::Get()->mySelectedObject->GetName();

		strncpy_s(nameBuffer, sizeof(nameBuffer), nameString.c_str(), _TRUNCATE);

		if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
		{
			MainSingleton::Get()->mySelectedObject->SetName(std::string(nameBuffer));
		}

		MainSingleton::Get()->mySelectedObject->RenderImGUI();

		if (ImGui::Button("MeshRenderer", ImVec2(200, 0)))
		{
			if (!MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>())
			{
				MainSingleton::Get()->mySelectedObject->AddComponent<MeshRenderer>();
			}
		}
		if (ImGui::Button("Light", ImVec2(200, 0)))
		{
			if (!MainSingleton::Get()->mySelectedObject->GetComponent<DirectionalLight>())
			{
				MainSingleton::Get()->mySelectedObject->AddComponent<DirectionalLight>();
			}
		}
		if (ImGui::Button("Animator", ImVec2(200, 0)))
		{
			if (!MainSingleton::Get()->mySelectedObject->GetComponent<Animator>())
			{
				MainSingleton::Get()->mySelectedObject->AddComponent<Animator>();
			}
		}

		if (ImGui::Button("Move", ImVec2(200, 0)))
		{
			if (!MainSingleton::Get()->mySelectedObject->GetComponent<Move>())
			{
				MainSingleton::Get()->mySelectedObject->AddComponent<Move>();
			}
		}

		if (ImGui::Button("Spin", ImVec2(200, 0)))
		{
			if (!MainSingleton::Get()->mySelectedObject->GetComponent<Spin>())
			{
				MainSingleton::Get()->mySelectedObject->AddComponent<Spin>();
			}
		}

		if (ImGui::Button("Reflection probe", ImVec2(200, 0)))
		{
			if (!MainSingleton::Get()->mySelectedObject->GetComponent<ReflectionProbe>())
			{
				MainSingleton::Get()->mySelectedObject->AddComponent<ReflectionProbe>();
			}
		}
	}

	ImGui::End();
}

void GameWorld::TopFileMenuItem()
{
	if (ImGui::MenuItem("Open", "Ctrl+O"))
	{
		//OPENFILENAME ofn = { 0 };

		//ofn.lStructSize = sizeof(ofn);
		//ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST;
		//ofn.hInstance = GetModuleHandle(0);
		//ofn.hwndOwner = NULL;
		//ofn.nMaxFile = MAX_PATH;
		//ofn.nFilterIndex = 1;
		//ofn.lpstrFile = ConvertStringToLPWSTR(MainSingleton::Get()->activeScene.name);
		//ofn.lpstrInitialDir = L"Scenes";
		//ofn.lpstrFilter = L"Json\0*.json\0";

		//if (GetOpenFileName(&ofn)) {
		//	// file name was selected, save the file...

		//	LPWSTR lpwstr = ofn.lpstrFile;
		//	std::string str = convertLPWSTRToString(lpwstr);

		//	MainSingleton::Get()->activeScene = SceneManagerInternal::LoadScene(str, globalModelsLoaded);
		//}
		//else {

		//}
	}
	if (ImGui::MenuItem("Save", "Ctrl+S"))
	{
		OPENFILENAME ofn = { 0 };

		ofn.lStructSize = sizeof(ofn);
		ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST;
		ofn.hInstance = GetModuleHandle(0);
		ofn.hwndOwner = NULL;
		ofn.nMaxFile = MAX_PATH;
		ofn.nFilterIndex = 1;

		static LPWSTR stringToLPWSTR = ConvertStringToLPWSTR(MainSingleton::Get()->activeScene->name);

		ofn.lpstrFile = stringToLPWSTR;
		ofn.lpstrInitialDir = L"Scenes";
		ofn.lpstrFilter = L"no\0*.no\0";

		if (GetSaveFileName(&ofn))
		{
			// file name was selected, save the file...

			static LPWSTR lpwstr = ofn.lpstrFile;
			static std::string str = convertLPWSTRToString(lpwstr);

			SceneManagerInternal::SaveScene(MainSingleton::Get()->activeScene, str);
		}
		else
		{
			// user cancelled
			printf("Save cancelled.\n");
		}
	}
}

void GameWorld::TopNewObjectMenuItem()
{
	if (ImGui::MenuItem("Empty Gameobject"))
	{
		MainSingleton::Get()->mySelectedObject = NewEmptyGameObject("Empty Gameobject");
	}
	if (ImGui::MenuItem("Cube"))
	{
		std::string Cube = "Assets/Models/Cube.fbx";

		MainSingleton::Get()->mySelectedObject = NewGameObject("Cube", Cube);
	}
	if (ImGui::BeginMenu("Sphere"))
	{
		if (ImGui::MenuItem("UVSphere"))
		{
			std::string uvSphere = "Assets/Models/UVSphere.fbx";

			MainSingleton::Get()->mySelectedObject = NewGameObject("UVSphere", uvSphere);
		}
		if (ImGui::MenuItem("IcoSphere"))
		{
			std::string icoSphere = "Assets/Models/IcoSphere.fbx";

			MainSingleton::Get()->mySelectedObject = NewGameObject("IcoSphere", icoSphere);
		}

		ImGui::EndMenu();
	}

	if (ImGui::MenuItem("Chest"))
	{
		std::string chest = "Assets/Models/Particle_Chest.fbx";

		MainSingleton::Get()->mySelectedObject = NewGameObject("Particle_Chest", chest);
	}
	if (ImGui::MenuItem("Monkey"))
	{
		std::string monke = "Assets/Models/Monkey.fbx";

		MainSingleton::Get()->mySelectedObject = NewGameObject("Monkey", monke);
	}
	if (ImGui::MenuItem("TGABro"))
	{
		std::string tGABro = "Assets/Models/SK_C_TGA_Bro.fbx";

		MainSingleton::Get()->mySelectedObject = NewGameObject("SK_C_TGA_Bro", tGABro);

		MainSingleton::Get()->mySelectedObject->AddComponent<Animator>();
	}
	if (ImGui::MenuItem("SDFTRMonster0001"))
	{
		std::string monster = "Assets/Models/SDFTRMonster0001.fbx";

		MainSingleton::Get()->mySelectedObject = NewGameObject("SDFTRMonster0001", monster);
	}
	if (ImGui::MenuItem("Shader ball"))
	{
		std::string shaderBall = "Assets/Models/MaterialTester.fbx";

		MainSingleton::Get()->mySelectedObject = NewGameObject("Shader ball", shaderBall);

		MeshRenderer* meshRenderer = MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>();

		meshRenderer->myMaterial.SetDiffuseTexture(L"Assets/Textures/Shader Ball JL 01_BaseColor.dds");
		meshRenderer->myMaterial.SetNormalTexture(L"Assets/Textures/Shader Ball JL 01_Normal.dds");
	}
	if (ImGui::MenuItem("Directional light"))
	{
		MainSingleton::Get()->mySelectedObject = NewEmptyGameObject("Directional light");

		MainSingleton::Get()->mySelectedObject->AddComponent<DirectionalLight>();
	}
}

void GameWorld::RenderTopMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			TopFileMenuItem();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("New"))
		{
			TopNewObjectMenuItem();
			ImGui::EndMenu();
		}

		ImGui::SameLine();

		if (ImGui::Button("Play"))
		{
			EnterPlayMode();
		}
		if (ImGui::Button("Pause"))
		{
			ExitPlayMode();
		}
		if (ImGui::Button("Stop"))
		{
			ExitPlayMode();
		}

		ImGui::EndMainMenuBar();
	}
}

void GameWorld::Begin()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_::ImGuiDockNodeFlags_PassthruCentralNode);

	ImGuizmo::BeginFrame();

	CommonUtilities::Time::Update();

	ImGui::Begin("FPS counter", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);
	ImGui::Text(("FPS: " + std::to_string(static_cast<int>(currentDisplayedFPS))).c_str());
	ImGui::End();

	PIXBeginEvent(PIX_COLOR_INDEX(2), L"Render top menu bar");
	RenderTopMenuBar();
	PIXEndEvent();

	if (!MainSingleton::Get()->myIsPlaying)
	{
		PIXBeginEvent(PIX_COLOR_INDEX(2), L"Render inspector");
		RenderInspector();
		PIXEndEvent();

		PIXBeginEvent(PIX_COLOR_INDEX(2), L"Render hierarchy");
		RenderHierarchy();
		PIXEndEvent();

		PIXBeginEvent(PIX_COLOR_INDEX(2), L"Render scene");
		SceneRender();
		PIXEndEvent();
	}
}

void GameWorld::EditorUpdate()
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), L"Update Engine");

	{
		PIXScopedEvent(PIX_COLOR_INDEX(1), L"Update Transforms");

		std::unordered_map<std::string, std::shared_ptr<GameObject>>::iterator iterator = MainSingleton::Get()->activeScene->gameObjects.begin();

		for (int i = 0; i < MainSingleton::Get()->activeScene->gameObjects.size(); i++, iterator++)
		{
			PIXScopedEvent(PIX_COLOR_INDEX(1), "update transform nr %llu", i);


			if (iterator->second->myToBeDeleted)
			{
				MainSingleton::Get()->activeScene->gameObjects.erase(iterator->first);
				continue;
			}

			const std::string& gameobjectName = iterator->first;

			PIXScopedEvent(PIX_COLOR_INDEX(1), (L"Update transform for: " + std::wstring(gameobjectName.begin(), gameobjectName.end())).c_str());

			iterator->second->transform->Update(CommonUtilities::Time::GetDeltaTime());
		}
	}

	if (MainSingleton::Get()->myIsPlaying)
	{
		PIXScopedEvent(PIX_COLOR_INDEX(1), L"Play mode update all components");

		PIXBeginEvent(PIX_COLOR_INDEX(1), L"Play mode early update components");
		MainSingleton::Get()->myComponentManager->UpdateComponentsEarly(CommonUtilities::Time::GetDeltaTime());
		PIXEndEvent();

		PIXBeginEvent(PIX_COLOR_INDEX(1), L"Play mode update components");
		MainSingleton::Get()->myComponentManager->UpdateComponents(CommonUtilities::Time::GetDeltaTime());
		PIXEndEvent();

		PIXBeginEvent(PIX_COLOR_INDEX(1), L"Play mode late update components");
		MainSingleton::Get()->myComponentManager->UpdateComponentsLate(CommonUtilities::Time::GetDeltaTime());
		PIXEndEvent();
	}
	{
		PIXScopedEvent(PIX_COLOR_INDEX(1), L"Editor mode update all components");

		PIXBeginEvent(PIX_COLOR_INDEX(1), L"Always early update components");
		MainSingleton::Get()->myComponentManager->EditorUpdateComponentsEarly(CommonUtilities::Time::GetDeltaTime());
		PIXEndEvent();

		PIXBeginEvent(PIX_COLOR_INDEX(1), L"Always update components");
		MainSingleton::Get()->myComponentManager->EditorUpdateComponents(CommonUtilities::Time::GetDeltaTime());
		PIXEndEvent();

		PIXBeginEvent(PIX_COLOR_INDEX(1), L"Always late update components");
		MainSingleton::Get()->myComponentManager->EditorUpdateComponentsLate(CommonUtilities::Time::GetDeltaTime());
		PIXEndEvent();
	}

	PIXBeginEvent(PIX_COLOR_INDEX(1), L"UpdateCamera");
	GraphicsEngine::Get().UpdateRender(CommonUtilities::Time::GetDeltaTime());

	PIXEndEvent();

	PIXBeginEvent(PIX_COLOR_INDEX(2), L"Render loop");

#ifdef MULTITHREADED
	deffRendering.myFinishedRendering = false;
#endif


#ifndef MULTITHREADED
	myDeffRendering.Render();
#endif
	PIXEndEvent();

	PIXScopedEvent(PIX_COLOR_INDEX(1), L"Render ImGUI");

	EditorRenderGUI();

	PhysicsEngine::Get().Update();
	}

void GameWorld::EditorRenderGUI()
{
	if (!MainSingleton::Get()->myIsPlaying)
	{
		inspectorCamera->UpdateAlways();

		if (!MainSingleton::Get()->mySpriteEditorOpen)
		{
			if (MainSingleton::Get()->mySelectedObject)
			{
				if (InputHandler::GetInstance()->GetKeyDown(keycode::DELETE_BUTTON))
				{
					std::shared_ptr<GameObject>& gameObject = MainSingleton::Get()->mySelectedObject;

					GameObject::Destroy(gameObject);

					MainSingleton::Get()->mySelectedObject = nullptr;
				}
			}
		}

		if (InputHandler::GetInstance()->GetKeyDown(keycode::F))
		{
			if (MainSingleton::Get()->mySelectedObject)
			{
				//Camera::main->gameObject->transform->GetFinalTransform().SetPosition(MainSingleton::Get()->mySelectedObject->transform->position - CommonUtilities::Vector4<float>(Camera::main->myForward).GetNormalized() * 1.f);
			}
		}

		/*if (InputHandler::GetInstance()->GetKeyDown(keycode::R))
		{
			for (auto& gameobject : MainSingleton::Get()->activeScene.gameObjects)
			{
				MeshRenderer* comp = gameobject->GetComponent<MeshRenderer>();

				if (comp)
				{
					Shaders::CompileShader(&comp->GetMaterial());
				}
			}
		}*/

		/*if (InputHandler::GetInstance()->GetKeyDown(keycode::P))
		{
			if (playMode)
			{
				ExitPlayMode();
			}
			else
			{
				EnterPlayMode();
			}
		}*/

		if (InputHandler::GetInstance()->GetKeyDown(keycode::W))
		{
			MainSingleton::Get()->gizmoOperation = ImGuizmo::TRANSLATE;
		}
		else if (InputHandler::GetInstance()->GetKeyDown(keycode::E))
		{
			MainSingleton::Get()->gizmoOperation = ImGuizmo::ROTATE;
		}
		else if (InputHandler::GetInstance()->GetKeyDown(keycode::R))
		{
			MainSingleton::Get()->gizmoOperation = ImGuizmo::SCALE;
		}

		// pixel picking
		if (InputHandler::GetInstance()->GetMouseButtonDown(0))
		{
			//GraphicsEngine::GetRHI()->GetPixelDX11(mousePos.x, mousePos.y, GraphicsEngine::GetRHI()->myDefferedPixelPickerSRV.Get());
		}

		{
			PIXScopedEvent(PIX_COLOR_INDEX(1), L"Sprite sheet update");
			// sprite editor
			if (MainSingleton::Get()->mySpriteEditorOpen)
				MainSingleton::Get()->spriteEditor->Update();
		}




		{
			PIXScopedEvent(PIX_COLOR_INDEX(1), L"Render console");
			Console::Render();
		}

		{
			PIXScopedEvent(PIX_COLOR_INDEX(1), L"Render asset tab");
			myAssetsTab.Render();
		}
	}
	else
	{
		gameCamera->UpdateAlways();
	}

	if (nextFPSUpdate++ >= 240)
	{
		currentDisplayedFPS = 1.f / CommonUtilities::Time::GetDeltaTime();

		nextFPSUpdate = 0;
	}

	GraphicsEngine::GetRHI()->SetBackBufferRT();

	ImGuiIO& io = ImGui::GetIO();

	io.ConfigWindowsMoveFromTitleBarOnly = true;

	RenderImGui(io);
}