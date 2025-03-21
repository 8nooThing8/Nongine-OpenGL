#include "../Engine.pch.h"

#include "MainSingleton.h"

#include "../Engine/GraphicsEngine/GraphicsCommandList.h"
#include "ShaderResources.h"
#include "SpriteEditor.h"

#include "NetworkManager.h"

#include <thread>

MainSingleton::MainSingleton(): mySkyBoxMeshRend(nullptr), mySkeletonBone(nullptr)
{
	gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	gizmoMode = ImGuizmo::MODE::LOCAL;

	myComponentManager = new ComponentManager();
	myDefferedCommandList = new GraphicsCommandList();
	myForwardCommandList = new GraphicsCommandList();
	myFXCommandList = new GraphicsCommandList();
	mySkeletonCommandList = new GraphicsCommandList();
	shaderResources = new ShaderResources();
	mySelectedObject = nullptr;
	spriteEditor = new SpriteEditor();
	myNetworkManager = new NetworkManager();
}

MainSingleton::~MainSingleton()
{
	delete myComponentManager;
	delete myDefferedCommandList;
	delete myForwardCommandList;
	delete myFXCommandList;
	delete mySkeletonCommandList;
	delete shaderResources;
	delete myNetworkManager;
}