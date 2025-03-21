#pragma once

#include <unordered_map>
#include <vector>
#include "memory"
#include "string"

#include "../PhysicsEngine/PhysicsEngine.h"
#include <functional>



class GameObject;


struct Scene
{
	std::string name;

	std::unordered_map<uint32_t, std::shared_ptr<GameObject>> gameObjects;

	PhysicsScene myPhysicsScene;

	Scene();

	Scene(std::string aName);

	Scene(Scene& aScene);

	std::shared_ptr<GameObject> NewGameObject(const std::string& aName, std::string& aPath, int id = 0) const;
	std::shared_ptr<GameObject> NewEmptyGameObject(const std::string& aName, int id = 0) const;
};

class SceneManagerInternal
{
public:
	SceneManagerInternal() = default;
	~SceneManagerInternal() = default;

	static void SaveScene(Scene* aSceneToBeSaved, std::string aPath = "Scenes/");
	static Scene* LoadScene(std::string aScenePath);
};