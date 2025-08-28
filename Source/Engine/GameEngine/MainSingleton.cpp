#include "MainSingleton.h"

#include "../Engine/GraphicsEngine/GraphicsCommandList.h"
#include "../Engine/GameEngine/ComponentManager.h"
#include "../Engine/GameEngine/NetworkManager.h"

MainSingleton::MainSingleton()
{
	myComponentManager = new ComponentManager();
	#ifdef USENET
		myNetworkManager = new NetworkManager();
	#endif
}

MainSingleton::~MainSingleton()
{
	delete myComponentManager;
}