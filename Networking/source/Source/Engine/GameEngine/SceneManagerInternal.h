#pragma once

#include <unordered_map>
#include <vector>
#include "memory"
#include "string"

class GameObject;

#include "../PhysicsEngine/PhysicsEngine.h"

struct Scene
{
	std::string name;

	std::unordered_map<std::string, std::shared_ptr<GameObject>> gameObjects;

	PhysicsScene myPhysicsScene;

	Scene() = default;

	Scene(std::string aName);

	Scene(Scene& aScene);
};

class SceneManagerInternal
{
public:
	SceneManagerInternal() = default;
	~SceneManagerInternal() = default;

	static void SaveScene(Scene* aSceneToBeSaved, std::string aPath = "Scenes/");
	static Scene* LoadScene(std::string aScenePath);
};