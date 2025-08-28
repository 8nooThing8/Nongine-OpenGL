#pragma once

#include <vector>
#include <queue>
#include <functional>

#include <unordered_map>

//#include "../PhysicsEngine\PhysicsScene.h"

class ComponentManager;
class NetworkManager;
class GraphicsCommandList;
class GameObject;

struct GLFWwindow;

class MainSingleton
{
public:
	MainSingleton();
	~MainSingleton();

	static MainSingleton& Get()
	{
		static MainSingleton instance;
		return instance;
	}

public:
	GLFWwindow* myWindow;
	ComponentManager* myComponentManager;
	//PhysicsScene myPhysicsScene;
	std::vector<GameObject*> mySelectedObjects;
	std::unordered_map<int, GameObject*> myGameObjects;
	
	#ifdef USENET
		NetworkManager* myNetworkManager;
		std::queue<std::function<void()>> myNetworkQueue;
	#endif
};
