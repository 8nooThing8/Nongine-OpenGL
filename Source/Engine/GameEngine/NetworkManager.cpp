#include "NetworkManager.h"

#include "../Engine/GameEngine/Components/Component.h"
#include "../Engine/GameEngine/Components/Networking/NetworkComponent.h"
#include "../Engine/GameEngine/MainSingleton.h"
#include "../Engine/GameEngine/SceneManagerInternal.h"

#include "../Networking/Client/Client.h"
#include "../Networking/Server/Server.h"

#include "../Engine/GameEngine/Components/GameObject.h"

#include "../Engine/GameEngine/Components/MeshRenderer.h"

#include "../Engine/GameEngine/Components/Networking/NetworkTransform.h"
#include "../Engine/GameEngine/Components/Animator.h"

#include <rapidjson-master/include/rapidjson/rapidjson.h>
#include <rapidjson-master/include/rapidjson/document.h>
#include <rapidjson-master/include/rapidjson/filereadstream.h>

#include "../Engine/GameEngine/Components/Physics/Colliders/CapsuleCollider.h"

#include "Hash.h"

NetworkManager::NetworkManager() : myClient(nullptr)
{

}

void NetworkManager::StartGame()
{
	myClient = new Client();
	int iResult = myClient->Init();
	iResult;

	myRecieveDataThread = std::thread(&NetworkManager::RecieveData, this);
	myRecieveDataThread.detach();
}

std::array<std::wstring, 3> LoadTexture(std::string aFilePath)
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

void NetworkManager::RecieveData()
{
	while (true)
	{
		NetworkRecieveData networkData = myClient->RecieveData();
	
		int hashedObject = Hash(networkData.objectID);

		if (!MainSingleton::Get()->activeScene->gameObjects.contains(hashedObject))
		{
			networkMutex.lock();

			std::string tgaBro = "Assets/Models/SK_C_TGA_Bro.fbx";

			MainSingleton::Get()->mySelectedObject = MainSingleton::Get()->activeScene->NewGameObject("TgaBro-Network", tgaBro, networkData.objectID);

			MainSingleton::Get()->mySelectedObject->transform->position = CommonUtilities::Vector4<float>(0, 100, 0, 1);
			MainSingleton::Get()->mySelectedObject->transform->rotation = CommonUtilities::Vector4<float>(0, 0, 0, 1);


			NetworkTransform* networkTransform = MainSingleton::Get()->mySelectedObject->AddComponent<NetworkTransform>();
			networkTransform->sendData = true;
			networkTransform->recieveData = true;

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

			MainSingleton::Get()->mySelectedObject->StartComponents();

			networkMutex.unlock();
		}
		
		auto gameobject = MainSingleton::Get()->activeScene->gameObjects[hashedObject];
		if (gameobject)
		{
			auto networkComponent = gameobject->GetComponents<NetworkComponent>();

			for (auto comp : networkComponent)
			{
				if (comp->myDataType == networkData.dataType)
				{
					comp->RecieveDataNetwork(networkData);
				}
			}
		}
	}
}

Client* NetworkManager::GetClient()
{
	return myClient;
}
