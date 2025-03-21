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
#include "../Engine/GameEngine/GameCamera.h"

#include "../Engine/GameEngine/Components/CameraComponent.h"
#include "../Engine/GameEngine/Components/MeshRenderer.h"
#include "../Engine/GameEngine/Components/SpriteRenderer.h"
#include "../Engine/GameEngine/Components/Move.hpp"
#include "../Engine/GameEngine/Components/Spin.hpp"
#include "../Engine/GameEngine/Components/Particles/ParticleSystem.h"
#include "../Engine/GameEngine/Components/Trail/TrailSystem.h"
#include "../Engine/GameEngine/Components/InstanceComponent.h"
#include "../Engine/GameEngine/Components/DirectionalLight.h"
#include "../Engine/GameEngine/Components/ReflectionProbe.h"
#include "../Engine/GameEngine/Components/Networking/NetworkTransform.h"
#include "../Engine/GameEngine/Components/Game/ThrowBallAtHouseComponent.h"

#include "../Engine/GameEngine/Components/Physics/Colliders/SphereCollider.h"
#include "../Engine/GameEngine/Components/Physics/Colliders/BoxCollider.h"
#include "../Engine/GameEngine/Components/Physics/Colliders/CapsuleCollider.h"
#include "../Engine/GameEngine/Components/Physics/Colliders/MeshCollider.h"
#include "../Engine/GameEngine/Components/Physics/CharacterController.h"
#include "../Engine/GameEngine/Components/Physics/RigidBody.h"

#include <random>

#include "../Engine/GameEngine/ModelSprite/Sprite.h"
#include "../ImGuizmo\ImGuizmo.h"
#include "../Engine/GameEngine/NetworkManager.h"

#include "../Engine/GameEngine/ComponentManager.h"

#include "../Engine/GameEngine/SpriteEditor.h"
#include "../Engine/GameEngine/Components/Animator.h"
#include "../Engine/GameEngine/Components/Transform.h"
#include "../Engine/GameEngine/ModelSprite/ModelFactory.h"

#include "../Engine/GameEngine/SceneManagerInternal.h"

#include "../PhysicsEngine/PhysicsEngine.h"

#include <pix3.h>
#include <../Engine/GameEngine/Components/Player.hpp>

#include "Hash.h"

#include "../Networking/Client/Client.h"

#include "../Engine/GameEngine/Components/TypeRegistry.h"

using namespace std;


GameWorld::GameWorld(HWND aHWND)
{
	inspectorCamera = new InspectorCamera();
	inspectorCamera->position = CommonUtilities::Vector4<float>(0, 100, -100, 1);

	myAssetsTab = AssetsTab();


	DefferedRendering::Get().Init(aHWND);

	MainSingleton::Get()->MainSingleton::Get()->activeScene = new Scene("Empty Scene");
}

//REGISTER_TYPE(CameraComponent)
REGISTER_TYPE(MeshRenderer)
REGISTER_TYPE(Animator)
REGISTER_TYPE(SpriteRenderer)
REGISTER_TYPE(Move)
REGISTER_TYPE(Spin)
REGISTER_TYPE(ParticleSystem)
REGISTER_TYPE(TrailSystem)
REGISTER_TYPE(InstanceComponent)
REGISTER_TYPE(DirectionalLight)
REGISTER_TYPE(ReflectionProbe)
REGISTER_TYPE(NetworkTransform)
REGISTER_TYPE(BoxCollider)
REGISTER_TYPE(CapsuleCollider)
REGISTER_TYPE(MeshCollider)
REGISTER_TYPE(CharacterController)
REGISTER_TYPE(RigidBody)
REGISTER_TYPE(ThrowBallAtHouseComponent)

void GameWorld::ChildChecker(std::vector<std::shared_ptr<GameObject>>& aChildren, const int aTotalIndent)
{
	for (int i = 0; i < aChildren.size(); i++)
	{
		ImGui::PushID(aChildren[i]->GetID());

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

void GameWorld::EnterPlayMode()
{
	MainSingleton::Get()->myIsPlaying = true;

	MainSingleton::Get()->myComponentManager->StartComponents();
	//MainSingleton::Get()->myNetworkManager->StartGame();

	gameCamera->SetActiveCameraAsThis();

	{
		/*mt19937 mt(static_cast<unsigned>(time(0)));
		uniform_int_distribution<int> dist(0, 999);

		std::string tgaBro = "Assets/Models/SK_C_TGA_Bro.fbx";

		MainSingleton::Get()->mySelectedObject = MainSingleton::Get()->activeScene->NewGameObject("TgaBro-Network", tgaBro, dist(mt));

		MainSingleton::Get()->mySelectedObject->transform->position = CommonUtilities::Vector4<float>(0, 0, 0, 1);
		MainSingleton::Get()->mySelectedObject->transform->rotation = CommonUtilities::Vector4<float>(0, 0, 0, 1);

		MainSingleton::Get()->mySelectedObject->AddComponent<NetworkTransform>();*/

		//Animator* animator = MainSingleton::Get()->mySelectedObject->AddComponent<Animator>();

		//std::string animationToBeSet = "Assets/Animations/A_C_TGA_Bro_Walk.fbx";

		//animator->SetAnimation(animationToBeSet);

		//animator->SetSecondAnimation("Assets/Animations/A_C_TGA_Bro_Run.fbx");

		//animator->AddLayeredAnimation("RightShoulder");
		//animator->SetLayeredAnimation("Assets/Animations/A_C_TGA_Bro_Idle_Wave.fbx", 0);

		//std::array<std::wstring, 3> textures = LoadTexture("Assets/MAterials/TGABroMaterial.json");

		//MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetColor(CommonUtilities::Vector4f(0.15f, 0.5f, 0.15f, 1.f));
		//MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetShader(L"Assets/Shaders/Deffered_PS.hlsl", L"Assets/Shaders/Default_VS.hlsl", textures[0], textures[1]);

		//MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetMetallicTexture(textures[2]);
		//MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetRoughnessTexture(textures[2]);
		//MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetAOTexture(textures[2]);

		//MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetBloomStrength(2.5f);

		//auto characterController = MainSingleton::Get()->mySelectedObject->AddComponent<CharacterController>();
		//characterController->SetRadius(40);
		//characterController->SetHalfHeigt(50);

		//characterController->SetCollisionLayer(CollisionLayer::Default);

		//MainSingleton::Get()->mySelectedObject->AddComponent<Player>();


		//MainSingleton::Get()->mySelectedObject->SetID(dist(mt));
	}

	//MainSingleton::Get()->mySelectedObject->StartComponents();

	//MainSingleton::Get()->mySelectedObject->GetComponent<NetworkTransform>()->ForceUpdateTransform();



	//MainSingleton::Get()->myNetworkManager->GetClient()->SendData(0, 0, DataTypeSent::ADDOBJECT, MainSingleton::Get()->mySelectedObject->GetRawID());

	//SceneManagerInternal::SaveScene(MainSingleton::Get()->activeScene, "Assets/Scenes/Play.no");
}
void GameWorld::ExitPlayMode()
{
	MainSingleton::Get()->myIsPlaying = false;
	//MainSingleton::Get()->activeScene = SceneManagerInternal::LoadScene("Assets/Scenes/Play.no");

	MainSingleton::Get()->myComponentManager->StopComponents();

	inspectorCamera->SetActiveCameraAsThis(true);
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

void GameWorld::SampleSceneCreation()
{
	{
		auto go = MainSingleton::Get()->activeScene->NewEmptyGameObject("Camera");

		go->transform->position = CommonUtilities::Vector4<float>(0, 0, 0, 1);
		go->transform->rotation = CommonUtilities::Vector4<float>(0, 0, 0, 1);
		go->transform->scale = CommonUtilities::Vector4<float>(1, 1, 1, 1);

		gameCamera = go->AddComponent<CameraComponent>();

		go->AddComponent<ThrowBallAtHouseComponent>();
	}

	// Directional light
	{
		MainSingleton::Get()->mySelectedObject = MainSingleton::Get()->activeScene->NewEmptyGameObject("Directional light");

		MainSingleton::Get()->mySelectedObject->transform->position = CommonUtilities::Vector4<float>(0.f, 100.f, -115.f, 1.f);
		MainSingleton::Get()->mySelectedObject->transform->rotation = CommonUtilities::Vector4<float>(CommonUtilities::DegToRad(45.f), CommonUtilities::DegToRad(-45.f), 0.f, 1.f);

		MainSingleton::Get()->mySelectedObject->AddComponent<DirectionalLight>();
		MeshRenderer* meshRenderer = MainSingleton::Get()->mySelectedObject->AddComponent<MeshRenderer>();

		meshRenderer->GetMaterial().SetShader(L"Assets/Shaders/UnLit_PS.hlsl", L"Assets/Shaders/Default_VS.hlsl");

		meshRenderer->myMesh = CreateModel("Assets/Models/IcoSphere.fbx");

		meshRenderer->SetCastShadow(false);
	}

	// Floor
	{
		std::string Cube = "Assets/Models/Plane.fbx";
		MainSingleton::Get()->mySelectedObject = MainSingleton::Get()->activeScene->NewGameObject("Cube", Cube);

		MainSingleton::Get()->mySelectedObject->transform->position = CommonUtilities::Vector4<float>(0, -5, 0, 1);
		MainSingleton::Get()->mySelectedObject->transform->scale = CommonUtilities::Vector4<float>(10000, 1, 10000, 1);

		MeshRenderer* meshRend = MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>();

		meshRend->GetMaterial().SetShader(L"Assets/Shaders/Deffered_PS.hlsl", L"Assets/Shaders/Default_VS.hlsl");
		meshRend->GetMaterial().SetColor(CommonUtilities::Vector4<float>(0.3f, 0.3f, 0.3f, 1.f));
		meshRend->SetLayer(0);

		auto collider = MainSingleton::Get()->mySelectedObject->AddComponent<BoxCollider>();
		collider->SetHalfExtents(CU::Vector3f(10000, 5, 10000));
	}

	/*for (int i = 0; i < 10; i++)
	{
		auto aGO = MainSingleton::Get()->activeScene->NewEmptyGameObject("House boolean1");

		mt19937 mt(i);
		uniform_int_distribution<int> rd(-700, 700);

		aGO->transform->position = CommonUtilities::Vector4f(rd(mt), -14.700f, rd(mt), 1.f);
		aGO->transform->scale = CommonUtilities::Vector4f(100, 100, 10, 1);
		aGO->transform->rotation = CommonUtilities::Vector4f(3.14159365258979f * 0.5f, 0, 0, 1);


		MeshRenderer* meshRend = aGO->AddComponent<MeshRenderer>();
		meshRend->myMesh = CreateModel("Assets/Models/CylinderTop.fbx");
		meshRend->GetMaterial().SetShader(L"Assets/Shaders/Deffered_PS.hlsl", L"Assets/Shaders/Default_VS.hlsl");
		meshRend->GetMaterial().SetColor(CommonUtilities::Vector4f(1, 0, 0, 0.3f));
		meshRend->RemoveLayer(1);

		auto sphereCollider = aGO->AddComponent<MeshCollider>();
		sphereCollider->SetMesh(CreateModel("Assets/Models/Cylinder.fbx"));
		sphereCollider->SetOutsideShape();
		sphereCollider->AddIgnoreCollider(sphereCollider);
	}*/

	//{
	//	auto aGO = MainSingleton::Get()->activeScene->NewEmptyGameObject("House boolean1");

	//	aGO->transform->position = CommonUtilities::Vector4f(0, 0, -2.479f - 35, 1);
	//	aGO->transform->rotation = CommonUtilities::Vector4f(0, 0, 0, 1);
	//	aGO->transform->scale = CommonUtilities::Vector4f(4, 4, 2.618f, 1);


	//	{
	//		MeshRenderer* meshRend = aGO->AddComponent<MeshRenderer>();
	//		meshRend->myMesh = CreateModel("Assets/Models/Cylinder.fbx");
	//		meshRend->SetDeffered(true);
	//		meshRend->GetMaterial().SetColor(CommonUtilities::Vector4f(0, 0, 1, 0.3f));
	//		meshRend->RemoveLayer(1);
	//	}

	//	auto sphereCollider = aGO->AddComponent<MeshCollider>();
	//	sphereCollider->SetMesh(CreateModel("Assets/Models/Cylinder.fbx"));
	//	sphereCollider->SetOutsideShape();
	//	sphereCollider->AddIgnoreCollider(sphereCollider);
	//}

	{
		std::string CabinWall = "Assets/Models/CabinWall.fbx";
		MainSingleton::Get()->mySelectedObject = MainSingleton::Get()->activeScene->NewGameObject("CabinWall1", CabinWall);

		MainSingleton::Get()->mySelectedObject->transform->position = CommonUtilities::Vector4<float>(0, 0, -35, 1);
		MainSingleton::Get()->mySelectedObject->transform->scale = CommonUtilities::Vector4<float>(5, 5, 5, 1);

		MeshRenderer* meshRend = MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>();

		meshRend->GetMaterial().SetShader(L"Assets/Shaders/Deffered_PS.hlsl", L"Assets/Shaders/Default_VS.hlsl");
		meshRend->GetMaterial().SetDiffuseTexture(L"Assets/Textures/CabinWallTexture.dds");
		meshRend->SetLayer(1);

		const auto bounds = meshRend->GetMesh()->BoxBounds;

		const auto SizeX = bounds.Max[0] - bounds.Min[0];
		const auto SizeY = bounds.Max[1] - bounds.Min[1];
		const auto SizeZ = bounds.Max[2] - bounds.Min[2];

		auto collider = MainSingleton::Get()->mySelectedObject->AddComponent<BoxCollider>();
		collider->SetHalfExtents(CU::Vector3f(SizeX, SizeY, SizeZ) * (MainSingleton::Get()->mySelectedObject->transform->scale * 0.5f).ToVector3());
		collider->SetPositionOffset(CU::Vector3f(0, SizeY, 0));
		collider->SetCollisionLayer(CollisionLayer::Cabin);
	}

	{
		std::string CabinWall = "Assets/Models/CabinWall.fbx";
		MainSingleton::Get()->mySelectedObject = MainSingleton::Get()->activeScene->NewGameObject("CabinWall2", CabinWall);

		MainSingleton::Get()->mySelectedObject->transform->position = CommonUtilities::Vector4<float>(-35, 0, 35 - 35, 1);
		MainSingleton::Get()->mySelectedObject->transform->rotation = CommonUtilities::Vector4<float>(-3.14159f, 1.57079f, -3.14159f, 1);
		MainSingleton::Get()->mySelectedObject->transform->scale = CommonUtilities::Vector4<float>(5, 5, 5, 1);

		MeshRenderer* meshRend = MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>();

		meshRend->GetMaterial().SetShader(L"Assets/Shaders/Deffered_PS.hlsl", L"Assets/Shaders/Default_VS.hlsl");
		meshRend->GetMaterial().SetDiffuseTexture(L"Assets/Textures/CabinWallTexture.dds");
		meshRend->SetLayer(2);

		const auto bounds = meshRend->GetMesh()->BoxBounds;

		const auto SizeX = bounds.Max[0] - bounds.Min[0];
		const auto SizeY = bounds.Max[1] - bounds.Min[1];
		const auto SizeZ = bounds.Max[2] - bounds.Min[2];

		auto collider = MainSingleton::Get()->mySelectedObject->AddComponent<BoxCollider>();
		collider->SetHalfExtents(CU::Vector3f(SizeX, SizeY, SizeZ) * (MainSingleton::Get()->mySelectedObject->transform->scale * 0.5f).ToVector3());
		collider->SetPositionOffset(CU::Vector3f(0, SizeY, 0));
		collider->SetCollisionLayer(CollisionLayer::Cabin);
	}

	{
		std::string CabinWall = "Assets/Models/CabinWall.fbx";
		MainSingleton::Get()->mySelectedObject = MainSingleton::Get()->activeScene->NewGameObject("CabinWall3", CabinWall);

		MainSingleton::Get()->mySelectedObject->transform->position = CommonUtilities::Vector4<float>(35, 0, 35 - 35, 1);
		MainSingleton::Get()->mySelectedObject->transform->rotation = CommonUtilities::Vector4<float>(-3.14159f, -1.57079f, -3.14159f, 1);
		MainSingleton::Get()->mySelectedObject->transform->scale = CommonUtilities::Vector4<float>(5, 5, 5, 1);

		MeshRenderer* meshRend = MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>();

		meshRend->GetMaterial().SetShader(L"Assets/Shaders/Deffered_PS.hlsl", L"Assets/Shaders/Default_VS.hlsl");
		meshRend->GetMaterial().SetDiffuseTexture(L"Assets/Textures/CabinWallTexture.dds");
		meshRend->SetLayer(3);

		const auto bounds = meshRend->GetMesh()->BoxBounds;

		const auto SizeX = bounds.Max[0] - bounds.Min[0];
		const auto SizeY = bounds.Max[1] - bounds.Min[1];
		const auto SizeZ = bounds.Max[2] - bounds.Min[2];

		auto collider = MainSingleton::Get()->mySelectedObject->AddComponent<BoxCollider>();
		collider->SetHalfExtents(CU::Vector3f(SizeX, SizeY, SizeZ) * (MainSingleton::Get()->mySelectedObject->transform->scale * 0.5f).ToVector3());
		collider->SetPositionOffset(CU::Vector3f(0, SizeY, 0));
		collider->SetCollisionLayer(CollisionLayer::Cabin);
	}

	{
		std::string CabinWall = "Assets/Models/CabinWall.fbx";
		MainSingleton::Get()->mySelectedObject = MainSingleton::Get()->activeScene->NewGameObject("CabinWall1", CabinWall);

		MainSingleton::Get()->mySelectedObject->transform->position = CommonUtilities::Vector4<float>(0, 0, 70 - 35, 1);
		MainSingleton::Get()->mySelectedObject->transform->rotation = CommonUtilities::Vector4<float>(-3.14159f, 0, -3.14159f, 1);
		MainSingleton::Get()->mySelectedObject->transform->scale = CommonUtilities::Vector4<float>(5, 5, 5, 1);

		MeshRenderer* meshRend = MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>();

		meshRend->GetMaterial().SetShader(L"Assets/Shaders/Deffered_PS.hlsl", L"Assets/Shaders/Default_VS.hlsl");
		meshRend->GetMaterial().SetDiffuseTexture(L"Assets/Textures/CabinWallTexture.dds");
		meshRend->SetLayer(4);

		const auto bounds = meshRend->GetMesh()->BoxBounds;

		const auto SizeX = bounds.Max[0] - bounds.Min[0];
		const auto SizeY = bounds.Max[1] - bounds.Min[1];
		const auto SizeZ = bounds.Max[2] - bounds.Min[2];

		auto collider = MainSingleton::Get()->mySelectedObject->AddComponent<BoxCollider>();
		collider->SetHalfExtents(CU::Vector3f(SizeX, SizeY, SizeZ) * (MainSingleton::Get()->mySelectedObject->transform->scale * 0.5f).ToVector3());
		collider->SetPositionOffset(CU::Vector3f(0, SizeY, 0));
		collider->SetCollisionLayer(CollisionLayer::Cabin);
	}

	{
		std::string CabinWall = "Assets/Models/CabinWall.fbx";
		MainSingleton::Get()->mySelectedObject = MainSingleton::Get()->activeScene->NewGameObject("CabinRoof1", CabinWall);

		MainSingleton::Get()->mySelectedObject->transform->position = CommonUtilities::Vector4<float>(31.1f, 44.f, 35.f - 35, 1);
		MainSingleton::Get()->mySelectedObject->transform->rotation = CommonUtilities::Vector4<float>(-2.258f, -1.57f, -3.14159f, 1);
		MainSingleton::Get()->mySelectedObject->transform->scale = CommonUtilities::Vector4<float>(5, 5, 5, 1);

		MeshRenderer* meshRend = MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>();

		meshRend->GetMaterial().SetShader(L"Assets/Shaders/Deffered_PS.hlsl", L"Assets/Shaders/Default_VS.hlsl");
		meshRend->GetMaterial().SetDiffuseTexture(L"Assets/Textures/CabinWallTexture.dds");
		meshRend->SetLayer(5);

		const auto bounds = meshRend->GetMesh()->BoxBounds;

		const auto SizeX = bounds.Max[0] - bounds.Min[0];
		const auto SizeY = bounds.Max[1] - bounds.Min[1];
		const auto SizeZ = bounds.Max[2] - bounds.Min[2];

		auto collider = MainSingleton::Get()->mySelectedObject->AddComponent<BoxCollider>();
		collider->SetHalfExtents(CU::Vector3f(SizeX, SizeY, SizeZ) * (MainSingleton::Get()->mySelectedObject->transform->scale * 0.5f).ToVector3());
		collider->SetPositionOffset(CU::Vector3f(0, SizeY, 0));
		collider->SetCollisionLayer(CollisionLayer::Cabin);
	}

	{
		std::string CabinWall = "Assets/Models/CabinWall.fbx";
		MainSingleton::Get()->mySelectedObject = MainSingleton::Get()->activeScene->NewGameObject("CabinRoof2", CabinWall);

		MainSingleton::Get()->mySelectedObject->transform->position = CommonUtilities::Vector4<float>(-31.f, 44.f, 35.f - 35, 1);
		MainSingleton::Get()->mySelectedObject->transform->rotation = CommonUtilities::Vector4<float>(-2.25f, 1.57f, -3.14159f, 1);
		MainSingleton::Get()->mySelectedObject->transform->scale = CommonUtilities::Vector4<float>(5, 5, 5, 1);

		MeshRenderer* meshRend = MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>();

		meshRend->GetMaterial().SetShader(L"Assets/Shaders/Deffered_PS.hlsl", L"Assets/Shaders/Default_VS.hlsl");
		meshRend->GetMaterial().SetDiffuseTexture(L"Assets/Textures/CabinWallTexture.dds");
		meshRend->SetLayer(6);

		const auto bounds = meshRend->GetMesh()->BoxBounds;

		const auto SizeX = bounds.Max[0] - bounds.Min[0];
		const auto SizeY = bounds.Max[1] - bounds.Min[1];
		const auto SizeZ = bounds.Max[2] - bounds.Min[2];

		auto collider = MainSingleton::Get()->mySelectedObject->AddComponent<BoxCollider>();
		collider->SetHalfExtents(CU::Vector3f(SizeX, SizeY, SizeZ) * (MainSingleton::Get()->mySelectedObject->transform->scale * 0.5f).ToVector3());
		collider->SetPositionOffset(CU::Vector3f(0, SizeY, 0));
		collider->SetCollisionLayer(CollisionLayer::Cabin);
	}

	{
		std::string CabinRoofFace = "Assets/Models/CabinRoofTopFace.fbx";
		MainSingleton::Get()->mySelectedObject = MainSingleton::Get()->activeScene->NewGameObject("CabinRoofFace1", CabinRoofFace);

		MainSingleton::Get()->mySelectedObject->transform->position = CommonUtilities::Vector4<float>(-0.4f, 44.f, 0.f - 35, 1);
		MainSingleton::Get()->mySelectedObject->transform->rotation = CommonUtilities::Vector4<float>(0, 0, 0, 1);
		MainSingleton::Get()->mySelectedObject->transform->scale = CommonUtilities::Vector4<float>(5.4f, 5, 4.916f, 1);

		MeshRenderer* meshRend = MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>();

		meshRend->GetMaterial().SetShader(L"Assets/Shaders/Deffered_PS.hlsl", L"Assets/Shaders/Default_VS.hlsl");
		meshRend->GetMaterial().SetDiffuseTexture(L"Assets/Textures/CabinWallTexture.dds");
		meshRend->SetLayer(1);

		//const auto bounds = meshRend->GetMesh()->BoxBounds;

		//const auto SizeX = bounds.Max[0] - bounds.Min[0];
		//const auto SizeY = bounds.Max[1] - bounds.Min[1];
		//const auto SizeZ = bounds.Max[2] - bounds.Min[2];

		auto collider = MainSingleton::Get()->mySelectedObject->AddComponent<MeshCollider>();
		collider->SetMesh("Assets/Models/TriangleCollider.fbx");
		//collider->SetHalfExtents(CU::Vector3f(SizeX, SizeY, SizeZ) * (MainSingleton::Get()->mySelectedObject->transform->scale * 0.5f).ToVector3());
		collider->SetPositionOffset(CU::Vector3f(0, -3.f, 0));
		collider->SetCollisionLayer(CollisionLayer::Cabin);
	}

	{
		std::string CabinRoofFace = "Assets/Models/CabinRoofTopFace.fbx";
		MainSingleton::Get()->mySelectedObject = MainSingleton::Get()->activeScene->NewGameObject("CabinRoofFace2", CabinRoofFace);

		MainSingleton::Get()->mySelectedObject->transform->position = CommonUtilities::Vector4<float>(-0.4f, 44.f, 70.f - 35, 1);
		MainSingleton::Get()->mySelectedObject->transform->rotation = CommonUtilities::Vector4<float>(0, 3.14159265358979f, 0, 1);
		MainSingleton::Get()->mySelectedObject->transform->scale = CommonUtilities::Vector4<float>(5.4f, 5, 4.916f, 1);

		MeshRenderer* meshRend = MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>();

		meshRend->GetMaterial().SetShader(L"Assets/Shaders/Deffered_PS.hlsl", L"Assets/Shaders/Default_VS.hlsl");
		meshRend->GetMaterial().SetDiffuseTexture(L"Assets/Textures/CabinWallTexture.dds");
		meshRend->SetLayer(4);

		//const auto bounds = meshRend->GetMesh()->BoxBounds;

		//const auto SizeX = bounds.Max[0] - bounds.Min[0];
		//const auto SizeY = bounds.Max[1] - bounds.Min[1];
		//const auto SizeZ = bounds.Max[2] - bounds.Min[2];

		auto collider = MainSingleton::Get()->mySelectedObject->AddComponent<MeshCollider>();
		collider->SetMesh("Assets/Models/TriangleCollider.fbx");
		//collider->SetHalfExtents(CU::Vector3f(SizeX, SizeY, SizeZ) * (MainSingleton::Get()->mySelectedObject->transform->scale * 0.5f).ToVector3());
		collider->SetPositionOffset(CU::Vector3f(0, -3.f, 0));
		collider->SetCollisionLayer(CollisionLayer::Cabin);
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

	if (GameCamera::main->myIsInspectorCamera)
	{
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
		if (obj.second)
		{
			ImGui::PushID(obj.first);

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

			if (ImGui::Button(obj.second->GetName().c_str()))
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
				ImGui::Button(obj.second->GetName().c_str(), ImVec2(ImGui::CalcTextSize(obj.second->GetName().c_str()).x + 7, 0));
				ImGui::PopStyleColor();

				ImGui::EndDragDropSource();
			}

			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SELECTED");

				if (payload)
				{
					assert(payload->DataSize == sizeof(int));

					uint32_t* numptr = reinterpret_cast<uint32_t*>(payload->Data);

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

		std::vector<std::string> componentsName{ "MeshRenderer" , "Light" , "Animator" , "Player" , "Move" , "Spin" , "Reflection probe" , "CharacterController" , "CapsuleCollider" };

		MainSingleton::Get()->mySelectedObject->RenderImGUI();


		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponentPopUp");

		ImGui::SameLine();

		if (ImGui::BeginPopup("AddComponentPopUp"))
		{
			ImGui::SeparatorText("Components");
			for (int i = 0; i < componentsName.size(); i++)
			{
				if (ImGui::Selectable(componentsName[i].c_str()))
				{
					switch (i)
					{
					case 0:
						MainSingleton::Get()->mySelectedObject->AddComponent<MeshRenderer>();
						break;
					case 1:
						MainSingleton::Get()->mySelectedObject->AddComponent<DirectionalLight>();
						break;
					case 2:
						MainSingleton::Get()->mySelectedObject->AddComponent<Animator>();
						break;
					case 3:
						MainSingleton::Get()->mySelectedObject->AddComponent<Player>();
						break;
					case 4:
						MainSingleton::Get()->mySelectedObject->AddComponent<Move>();
						break;
					case 5:
						MainSingleton::Get()->mySelectedObject->AddComponent<Spin>();
						break;
					case 6:
						MainSingleton::Get()->mySelectedObject->AddComponent<ReflectionProbe>();
						break;
					case 7:
					{
						auto characterController = MainSingleton::Get()->mySelectedObject->AddComponent<CharacterController>();
						characterController->SetRadius(40);
						characterController->SetHalfHeigt(50);

						characterController->SetCollisionLayer(CollisionLayer::Default);
						break;
					}
					case 8:
					{
						auto capCollider = MainSingleton::Get()->mySelectedObject->AddComponent<CapsuleCollider>();

						capCollider->SetRadius(40);
						capCollider->SetHalfHeight(40);
						capCollider->SetPositionOffset(CommonUtilities::Vector3f(80, 0, 0));
						break;
					}
					}
				}
			}

			ImGui::EndPopup();
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
		//ofn.lpstrFile = ConvertStringToLPWSTR(MainSingleton::Get()->activeScene->name);
		//ofn.lpstrInitialDir = L"Scenes";
		//ofn.lpstrFilter = L"no\0*.no\0json\0*.json\0";

		//if (GetOpenFileName(&ofn)) {
		//	// file name was selected, save the file...

		//	LPWSTR lpwstr = ofn.lpstrFile;
		//	std::string str = convertLPWSTRToString(lpwstr);

		//	MainSingleton::Get()->activeScene = SceneManagerInternal::LoadScene(str);
		//}
		//else 
		//{

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
	bool CreatedMeshObject = false;

	if (ImGui::MenuItem("Empty Gameobject"))
	{
		MainSingleton::Get()->mySelectedObject = MainSingleton::Get()->activeScene->NewEmptyGameObject("Empty Gameobject");
	}
	if (ImGui::MenuItem("Cube"))
	{
		CreatedMeshObject = true;

		std::string Cube = "Assets/Models/Cube.fbx";

		MainSingleton::Get()->mySelectedObject = MainSingleton::Get()->activeScene->NewGameObject("Cube", Cube);
	}
	if (ImGui::BeginMenu("Sphere"))
	{
		if (ImGui::MenuItem("UVSphere"))
		{
			CreatedMeshObject = true;

			std::string uvSphere = "Assets/Models/UVSphere.fbx";

			MainSingleton::Get()->mySelectedObject = MainSingleton::Get()->activeScene->NewGameObject("UVSphere", uvSphere);
		}
		if (ImGui::MenuItem("IcoSphere"))
		{
			CreatedMeshObject = true;

			std::string icoSphere = "Assets/Models/IcoSphere.fbx";

			MainSingleton::Get()->mySelectedObject = MainSingleton::Get()->activeScene->NewGameObject("IcoSphere", icoSphere);
		}

		ImGui::EndMenu();
	}
	if (ImGui::MenuItem("Log"))
	{
		CreatedMeshObject = true;

		std::string Cube = "Assets/Models/CabinWall.fbx";

		MainSingleton::Get()->mySelectedObject = MainSingleton::Get()->activeScene->NewGameObject("CabinWall", Cube);
	}
	if (ImGui::MenuItem("Chest"))
	{
		CreatedMeshObject = true;

		std::string chest = "Assets/Models/Particle_Chest.fbx";

		MainSingleton::Get()->mySelectedObject = MainSingleton::Get()->activeScene->NewGameObject("Particle_Chest", chest);
	}
	if (ImGui::MenuItem("Monkey"))
	{
		CreatedMeshObject = true;

		std::string monke = "Assets/Models/Monkey.fbx";

		MainSingleton::Get()->mySelectedObject = MainSingleton::Get()->activeScene->NewGameObject("Monkey", monke);
	}
	if (ImGui::MenuItem("TGABro"))
	{
		CreatedMeshObject = true;

		std::string tGABro = "Assets/Models/SK_C_TGA_Bro.fbx";

		MainSingleton::Get()->mySelectedObject = MainSingleton::Get()->activeScene->NewGameObject("SK_C_TGA_Bro", tGABro);
	}
	if (ImGui::MenuItem("Shader ball"))
	{
		CreatedMeshObject = true;

		std::string shaderBall = "Assets/Models/MaterialTester.fbx";

		MainSingleton::Get()->mySelectedObject = MainSingleton::Get()->activeScene->NewGameObject("Shader ball", shaderBall);
	}
	if (ImGui::MenuItem("Directional light"))
	{
		MainSingleton::Get()->mySelectedObject = MainSingleton::Get()->activeScene->NewEmptyGameObject("Directional light");

		MainSingleton::Get()->mySelectedObject->AddComponent<DirectionalLight>();
	}

	if (CreatedMeshObject)
	{
		MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetShader(L"Assets/Shaders/Deffered_PS.hlsl", L"Assets/Shaders/Default_VS.hlsl");
		MainSingleton::Get()->mySelectedObject->GetComponent<MeshRenderer>()->GetMaterial().SetColor(CommonUtilities::Vector4<float>(1.f, 1.f, 1.f, 1.f));
	}
}

void GameWorld::ShortCuts()
{
	if (InputHandler::GetInstance()->GetKey(keycode::CONTROL))
	{
		if (InputHandler::GetInstance()->GetKeyDown(keycode::C))
		{
			MainSingleton::Get()->myCopiedObjects.clear();

			MainSingleton::Get()->myCopiedObjects.emplace_back(MainSingleton::Get()->mySelectedObject);
		}
	}

	if (InputHandler::GetInstance()->GetKey(keycode::CONTROL))
	{
		if (InputHandler::GetInstance()->GetKeyDown(keycode::V))
		{
			for (auto& copiedObject : MainSingleton::Get()->myCopiedObjects)
			{
				copiedObject->DuplicateObject();
			}
		}
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
		if (ImGui::Button("FullScreen"))
		{
			MainSingleton::Get()->fullscreenPlaying = !MainSingleton::Get()->fullscreenPlaying;
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

	if (!MainSingleton::Get()->fullscreenPlaying)
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
	else
		GraphicsEngine::Get().myGameResolution = CommonUtilities::Vector2<float>(1920, 1080);
}

void GameWorld::EditorUpdate()
{
	ShortCuts();

	PIXScopedEvent(PIX_COLOR_INDEX(1), L"Update Engine");

	{
		PIXScopedEvent(PIX_COLOR_INDEX(1), L"Update Transforms");

		std::unordered_map<uint32_t, std::shared_ptr<GameObject>>::iterator iterator = MainSingleton::Get()->activeScene->gameObjects.begin();

		for (int i = 0; i < MainSingleton::Get()->activeScene->gameObjects.size(); i++, iterator++)
		{
			PIXScopedEvent(PIX_COLOR_INDEX(1), "update transform nr %llu", i);

			if (iterator->second)
			{
				if (iterator->second->myToBeDeleted)
				{
					MainSingleton::Get()->activeScene->gameObjects.erase(iterator->first);
					continue;
				}

				const std::string& gameobjectName = iterator->second->GetName();

				PIXScopedEvent(PIX_COLOR_INDEX(1), (L"Update transform for: " + std::wstring(gameobjectName.begin(), gameobjectName.end())).c_str());

				iterator->second->transform->Update(CommonUtilities::Time::GetDeltaTime());
			}
		}
	}

	MainSingleton::Get()->myNetworkManager->networkMutex.lock();
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

	MainSingleton::Get()->myNetworkManager->networkMutex.unlock();

	PIXBeginEvent(PIX_COLOR_INDEX(1), L"UpdateCamera");
	GraphicsEngine::Get().UpdateRender(CommonUtilities::Time::GetDeltaTime());

	PIXEndEvent();

	PIXBeginEvent(PIX_COLOR_INDEX(2), L"Render loop");

#ifdef MULTITHREADED
	deffRendering.myFinishedRendering = false;
#endif


#ifndef MULTITHREADED
	DefferedRendering::Get().Render();
#endif
	PIXEndEvent();

	PIXScopedEvent(PIX_COLOR_INDEX(1), L"Render ImGUI");

	EditorRenderGUI();

	PhysicsEngine::Get().Update();
}

void GameWorld::EditorRenderGUI()
{
	GameCamera::main->UpdateAlways();

	if (!MainSingleton::Get()->fullscreenPlaying)
	{
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