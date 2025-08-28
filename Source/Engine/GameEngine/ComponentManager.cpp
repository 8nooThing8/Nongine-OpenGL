#include "ComponentManager.h"

#include "GameCamera.h"

#include "../Engine/GraphicsEngine/GraphicsCommandList.h"

#ifdef USENET
#include "NetworkManager.h"
#include "Networking/Shared/NetRole.h"
#endif

#include "Components/Transform.h"
#include "Components/GameObject.h"
#include <ranges>
#include "Components/Sprite2DRenderer.h"
#include "Components/Game/MoveServerObject.h"

#include <Primitive/AABB3D.hpp>
#include <Primitive/Sphere.hpp>
#include <Primitive/Ray.hpp>
#include <Shared/Common.h>

ComponentManager::~ComponentManager()
{
	for (auto& val : myComponents)
	{
		for (auto& comp : val.second)
		{
			delete comp;
		}
	}
}

void ComponentManager::AddSpritesToGraphicsEngine()
{
	if (myComponents.find("Sprite2DRenderer") == myComponents.end())
		return;

	auto& component = myComponents.at("Sprite2DRenderer");

	for (const auto& comp : component)
	{
		if (comp->myToBeDeleted)
			continue;
		static_cast<Sprite2DRenderer*>(comp)->AddToGraphicsEngine();
	}
}

bool ComponentManager::Raycast(const CU::Ray<float>& aRay, float aDistance)
{
	bool result = false;

	CU::AABB3D spriteCollider;
	
	auto& gameobjects = MainSingleton::Get().myGameObjects;
	for (auto& [_, gameobject] : gameobjects)
	{
		if (gameobject->GetName() == "Player")
			continue;

		CU::Vector4f scale = gameobject->transform->GetScale();
		const CU::Vector4f position = gameobject->transform->GetPosition();

		spriteCollider.SetPosition(position);
		spriteCollider.SetHalfExtents(CU::Vector3f(scale.x * 0.5f, scale.y * 0.5f, 0.f));
		result = CU::IntersectionAABBRay(spriteCollider, aRay, aDistance);

		if (result)
			return result;
	}

	return result;
}

void ComponentManager::StartComponents() const
{
	for (auto& component : myComponents)
	{
		for (auto& comp : component.second)
		{
			comp->Start();
		}
	}
}

void ComponentManager::DeleteComponents() 
{
	for (auto& component : myComponents)
	{
		for (int i = 0; i < component.second.size(); i++)
		{
			if (component.second[i]->myToBeDeleted)
			{
				delete component.second[i];
				component.second[i] = component.second.back();
				component.second.pop_back();
			}
		}
	}
}

void ComponentManager::UpdateComponents(const float aDeltaTime)
{
	for (auto& component : myComponents)
	{
		for (int i = 0; i < component.second.size(); i++)
		{
			if (component.second[i]->myToBeDeleted)
			{
				delete component.second[i];
				component.second[i] = component.second.back();
				component.second.pop_back();
			}
			else
			{
				component.second[i]->Update(aDeltaTime);
			}
		}
	}
}

void ComponentManager::EarlyUpdateComponents(const float aDeltaTime)
{
	for (auto& component : myComponents)
	{
		for (int i = 0; i < component.second.size(); i++)
		{
			if (component.second[i]->myToBeDeleted)
			{
				delete component.second[i];
				component.second[i] = component.second.back();
				component.second.pop_back();
			}
			else
			{
				component.second[i]->gameObject->transform->Update(aDeltaTime);
				component.second[i]->EarlyUpdate(aDeltaTime);
			}

		}
	}
}
