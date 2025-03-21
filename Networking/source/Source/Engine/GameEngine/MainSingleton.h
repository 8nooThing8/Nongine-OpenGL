#pragma once
#include "memory"

#include "../../Engine/GameEngine/Components/InstanceComponent.h"

class ShaderResources;
struct ID3D11ShaderResourceView;

class ComponentManager;
class GameObject;
class GraphicsCommandList;
class SpriteEditor;
class MeshRenderer;
class Transform;
class NetworkManager;

class NetRole;

struct Scene;

namespace ImGuizmo
{
	enum OPERATION: unsigned;
	enum MODE : unsigned;
}

namespace TGA
{
	namespace FBX
	{
		struct Mesh;
	}
}

class MainSingleton
{
public:
	MainSingleton();

	~MainSingleton();

	static MainSingleton* Get()
	{
		if (!Instance)
		{
			Instance = new MainSingleton();
		}

		return Instance;
	}

private:
	static inline MainSingleton* Instance = nullptr;

public:
	ImGuizmo::OPERATION gizmoOperation;
	ImGuizmo::MODE gizmoMode;

	NetworkManager* myNetworkManager;

	GraphicsCommandList* myDefferedCommandList;
	GraphicsCommandList* myForwardCommandList;

	GraphicsCommandList* myFXCommandList;

	GraphicsCommandList* mySkeletonCommandList;

	ComponentManager* myComponentManager;
	std::shared_ptr<GameObject> mySelectedObject;

	ShaderResources* shaderResources;

	std::shared_ptr<GameObject> mySkyBox;
	MeshRenderer* mySkyBoxMeshRend;

	InstanceComponent* instanceComponent;

	SpriteEditor* spriteEditor;

	Transform* objectToAttach;

	Scene* activeScene;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skyBoxTexture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myLUTTexture = nullptr;

	bool myIsPlaying = false;

	bool mySpriteEditorOpen = false;

	// Skeleton
	TGA::FBX::Mesh* mySkeletonBone;
};