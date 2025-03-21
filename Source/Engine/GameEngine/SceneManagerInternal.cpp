#include "../Engine.pch.h"

#include "SceneManagerInternal.h"

#include <rapidjson-master/include/rapidjson/filewritestream.h>
#include <rapidjson-master/include/rapidjson/filereadstream.h>
#include <rapidjson-master/include/rapidjson/document.h>
#include "rapidjson-master/include/rapidjson/prettywriter.h"

#include "../Engine/GameEngine/Components/TypeRegistry.h"

#include "../Engine/GameEngine/Components/GameObject.h"

#include "wingdi.h"

#include "../GameEngine/MainSingleton.h"

#include "../GameEngine/Components/GameObject.h"

#include "Console.h"

#include <cstdio>

#include "../GameEngine/Components/MeshRenderer.h"
#include "ModelSprite/ModelFactory.h"

#include "../GameEngine/Components/DirectionalLight.h"

#include "string"
#include <memory>

#include "Components\Transform.h"

#include "../GraphicsEngine/Shader.h"
#include "Components\Animator.h"

#include "../GraphicsEngine/ShaderCompiler.h"

#include "Hash.h"

#undef GetObject

void SceneManagerInternal::SaveScene(Scene* aSceneToBeSaved, std::string aPath)
{
	rapidjson::Document document;
	document.SetObject();

	rapidjson::Value obj(rapidjson::kObjectType);

	rapidjson::Value sceneNameInJsonFormat(aSceneToBeSaved->name.c_str(), document.GetAllocator());

	document.AddMember("name", sceneNameInJsonFormat, document.GetAllocator());

	for (auto& gameobject : aSceneToBeSaved->gameObjects)
	{
		std::shared_ptr<GameObject> currentGameObject = gameobject.second;
		std::string nName = currentGameObject->GetName();
		rapidjson::Value jsonName(nName.c_str(), document.GetAllocator());
		rapidjson::Value object(rapidjson::kObjectType);
		object.AddMember("name", jsonName, document.GetAllocator());
		rapidjson::Value Components(rapidjson::kObjectType);

		rapidjson::Value transform(rapidjson::kObjectType);
		gameobject.second->transform->HandleSave(transform, document);
		object.AddMember("Transform", transform, document.GetAllocator());

		for (auto& component : currentGameObject->GetComponents())
		{
			if (!component)
				continue;

			rapidjson::Value componentObject(rapidjson::kObjectType);
			component->HandleSave(componentObject, document);
			rapidjson::GenericStringRef name(component->myName.c_str());
			Components.AddMember(name, componentObject, document.GetAllocator());
		}

		object.AddMember("Components", Components, document.GetAllocator());
		obj.AddMember("Object", object, document.GetAllocator());
	}

	document.AddMember("Gameobjects", obj, document.GetAllocator());

	char writeBuffer[65536];
	FILE* fp;

	errno_t err;

	std::string name = aPath.substr(aPath.find_last_of(".") + 1);

	std::string test = aPath + ".no";

	if (name == "no")
	{
		err = fopen_s(&fp, (aPath).c_str(), "w");
	}
	else
	{
		err = fopen_s(&fp, (aPath + ".no").c_str(), "w");
	}


	if (err != 0)
	{
		PrintError("could not save file to: " + aPath + aSceneToBeSaved->name + ".no");

		return;
	}

	rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));

	rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);

	document.Accept(writer);

	fclose(fp);
}

Scene* SceneManagerInternal::LoadScene(std::string aScenePath)
{
	MainSingleton::Get()->myComponentManager->myComponents.clear();

	Scene* newScene = new Scene();

	MainSingleton::Get()->activeScene = newScene;

	FILE* fp;
	char readBuffer[65536];

	// Open the file containing the saved scene
	errno_t err = fopen_s(&fp, aScenePath.c_str(), "r");
	if (err != 0)
	{
		PrintError("could not find file to: " + aScenePath);
		throw std::runtime_error("Failed to open the scene file.");
	}

	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

	rapidjson::Document document;
	document.ParseStream(is);

	fclose(fp);

	if (!document.IsObject()) {
		throw std::runtime_error("Invalid JSON format: root should be an object.");
	}

	if (document.HasMember("name"))
	{
		newScene->name = document["name"].GetString();
	}

	const rapidjson::Value& objects = document["Gameobjects"];
	if (!objects.IsObject())
	{
		throw std::runtime_error("Invalid JSON format: missing 'Gameobjects' object.");
	}

	for (auto& object : objects.GetObject())
	{
		const rapidjson::Value& objectValue = object.value;

		std::string objectName = objectValue["name"].GetString();

		std::shared_ptr<GameObject> gameObject = newScene->NewEmptyGameObject(objectName);

		gameObject->transform->HandleLoad(objectValue["Transform"]);

		for (auto& component : objectValue["Components"].GetObject())
		{
			std::string componentName = component.name.GetString();
			auto obj = TypeRegistry::instance().create(componentName);
			if (obj)
			{
				auto& componentValue = component.value;
				obj->HandleLoad(componentValue);

				gameObject->AddComponent(obj.release(), componentName);
			}
		}
	}

	return newScene;
}

Scene::Scene()
{
	PhysicsEngine::Get().NewScene(myPhysicsScene);
}

Scene::Scene(std::string aName)
{
	name = aName;
	PhysicsEngine::Get().NewScene(myPhysicsScene);
}

Scene::Scene(Scene& aScene)
{
	for (auto& gameobject : aScene.gameObjects)
	{
		gameObjects.emplace(gameobject.first, gameobject.second);
	}

	name = aScene.name;
	PhysicsEngine::Get().NewScene(myPhysicsScene);
}

std::shared_ptr<GameObject> Scene::NewGameObject(const std::string& aName, std::string& aPath, int id) const
{
	auto gameobject = NewEmptyGameObject(aName, id);

	MeshRenderer* meshRenderer = gameobject->AddComponent<MeshRenderer>();

	Shaders::CompileShader(&meshRenderer->GetMaterial());

	meshRenderer->myDeffered = true;
	meshRenderer->myMesh = CreateModel(aPath);

	return MainSingleton::Get()->activeScene->gameObjects[gameobject->GetID()];
}

std::shared_ptr<GameObject> Scene::NewEmptyGameObject(const std::string& aName, int id) const
{
	std::shared_ptr<GameObject> gameobject = std::make_shared<GameObject>(aName);

	if (id != 0)
		gameobject->SetID(id);

	MainSingleton::Get()->activeScene->gameObjects.emplace(gameobject->GetID(), gameobject);

	return gameobject;
}
