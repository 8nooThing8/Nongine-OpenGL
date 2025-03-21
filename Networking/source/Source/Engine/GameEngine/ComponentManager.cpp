#include "../Engine.pch.h"

#include "ComponentManager.h"

#include "Components/MeshRenderer.h"
#include "Components/SpriteRenderer.h"

#include "GameCamera.h"
#include "Components/ReflectionProbe.h"

#include <ranges>

#include "Components/Transform.h"

ComponentManager::~ComponentManager()
{
	for (auto& val : myComponents | std::views::values)
	{
		for (auto& comp : val)
		{
			delete comp;
		}
	}
}

void ComponentManager::CleanUp()
{
	for (auto& meshRenderer : myComponents["MeshRenderer"])
	{
		dynamic_cast<MeshRenderer*>(meshRenderer)->ShaderCleanUp();
	}
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

void ComponentManager::StopComponents() const
{
	for (auto& component : myComponents)
	{
		for (int i = 0; i < component.second.size(); i++)
		{
			component.second[i]->Stop();
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
				if (component.second[i]->enabled && component.second[i]->gameObject->GetActive())
					component.second[i]->Update(aDeltaTime);
			}

		}
	}
}

void ComponentManager::UpdateComponentsEarly(const float aDeltaTime)
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
				if (component.second[i]->enabled && component.second[i]->gameObject->GetActive())
					component.second[i]->EarlyUpdate(aDeltaTime);
			}

		}
	}
}

void ComponentManager::UpdateComponentsLate(const float aDeltaTime)
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
				if (component.second[i]->enabled && component.second[i]->gameObject->GetActive())
					component.second[i]->LateUpdate(aDeltaTime);
			}

		}
	}
}

void ComponentManager::EditorUpdateComponents(const float aDeltaTime)
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
				if (component.second[i]->enabled && component.second[i]->gameObject->GetActive())
					component.second[i]->UpdateEditor(aDeltaTime);
			}

		}
	}
}

struct ForwardSorting
{
	inline bool operator() (Component* co1, Component* co2)
	{
		float distanceToCamera1;
		float distanceToCamera2;

		distanceToCamera1 = (co1->gameObject->transform->position - GameCamera::main->position).Length();
		distanceToCamera2 = (co2->gameObject->transform->position - GameCamera::main->position).Length();

		return distanceToCamera1 > distanceToCamera2;
	}
};

struct DefferedSorting
{
	inline bool operator() (Component* co1, Component* co2)
	{
		float distanceToCamera1;
		float distanceToCamera2;

		distanceToCamera1 = (co1->gameObject->transform->position - GameCamera::main->position).Length();
		distanceToCamera2 = (co2->gameObject->transform->position - GameCamera::main->position).Length();

		return distanceToCamera1 < distanceToCamera2;
	}
};

void ComponentManager::EditorUpdateComponentsEarly(const float aDeltaTime)
{
	std::vector<MeshRenderer*> meshRenderersDeffered;
	std::vector<MeshRenderer*> meshRenderersForward;

	for (auto& meshRenderer : myComponents["MeshRenderer"])
	{
		if (dynamic_cast<MeshRenderer*>(meshRenderer)->myDeffered)
		{
			meshRenderersDeffered.emplace_back(dynamic_cast<MeshRenderer*>(meshRenderer));
		}
		else
		{
			meshRenderersForward.emplace_back(dynamic_cast<MeshRenderer*>(meshRenderer));
		}
	}

	std::sort(meshRenderersDeffered.begin(), meshRenderersDeffered.end(), DefferedSorting());
	std::sort(meshRenderersForward.begin(), meshRenderersForward.end(), ForwardSorting());

	myComponents["MeshRenderer"].clear();

	myComponents["MeshRenderer"].insert(myComponents["MeshRenderer"].end(), meshRenderersForward.begin(), meshRenderersForward.end());
	myComponents["MeshRenderer"].insert(myComponents["MeshRenderer"].end(), meshRenderersDeffered.begin(), meshRenderersDeffered.end());

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
				if (component.second[i]->enabled && component.second[i]->gameObject->GetActive())
					component.second[i]->EarlyUpdateEditor(aDeltaTime);
			}

		}
	}
}

// All objects render with this
void ComponentManager::EditorUpdateComponentsLate(const float aDeltaTime)
{
	for (auto& component : myComponents)
	{
		if (component.first == "SpriteRenderer")
		{
			continue;
		}

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
				if (component.second[i]->enabled && component.second[i]->gameObject->GetActive())
					component.second[i]->LateUpdateEditor(aDeltaTime);
			}

		}
	}
}

void ComponentManager::RenderMeshOverride(MeshRenderer* notRenderThis, Microsoft::WRL::ComPtr<ID3D11PixelShader> shaderOverride)
{
	for (auto& meshRenderer : myComponents["MeshRenderer"])
	{
		if (notRenderThis != meshRenderer)
		{
			dynamic_cast<MeshRenderer*>(meshRenderer)->Draw(shaderOverride);
		}
	}
}

void ComponentManager::RenderSprite(SpriteRenderer* notRenderThis)
{
	for (auto& spriteRenderer : myComponents["SpriteRenderer"])
	{
		if (notRenderThis != spriteRenderer)
		{
			dynamic_cast<SpriteRenderer*>(spriteRenderer)->Draw();
		}
	}
}

void ComponentManager::LightOverride()
{
	for (auto& directionalLight : myComponents["DirectionalLight"])
	{
		directionalLight->EarlyUpdateEditor(0);
	}
}

void ComponentManager::ReflectionOverride(ReflectionProbe* aProbe, const float count)
{
	for (auto& reflectionProbe : myComponents["ReflectionProbe"])
	{
		if (reflectionProbe != aProbe)
		{
			reflectionProbe->EarlyUpdateEditor(count);
		}
	}
}