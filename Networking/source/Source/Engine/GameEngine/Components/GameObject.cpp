#include "../../Engine.pch.h"

#include "GameObject.h"
#include <ImGui/imgui.h>

#include "Transform.h"
#include "Component.h"

#include "../Engine/GameEngine/SceneManagerInternal.h"

GameObject::GameObject(): myParent(nullptr), myName("NoNameObject")
{
	myID = nextID++;

	Transform* tf = new Transform();

	tf->gameObject = this;

	transform = tf;

	myToBeDeleted = false;
}

GameObject::GameObject(const std::string& aName): myParent(nullptr), myName(aName)
{
	myID = nextID++;

	Transform* tf = new Transform();

	tf->gameObject = this;

	transform = tf;

	myToBeDeleted = false;
}

GameObject::~GameObject()
{
	MainSingleton::Get()->mySelectedObject = nullptr;
}

void GameObject::SetActive(bool aActive)
{
	myIsActive = aActive;
}


void GameObject::Destroy(std::shared_ptr<GameObject>& aGameobject)
{
	for (int i = static_cast<int>(aGameobject->GetChildren().size() - 1); i >= 0; i--)
	{
		std::shared_ptr<GameObject>& gameObject = aGameobject->GetChildren()[i];

		Destroy(gameObject);
		aGameobject->GetChildren().pop_back();
	}

	for (auto& component : aGameobject->GetComponents())
	{
		component->myToBeDeleted = true;
	}

	aGameobject->myToBeDeleted = true;
}


bool& GameObject::GetActive()
{
	return myIsActive;
}

void GameObject::RenderImGUI()
{
	ImGui::Text("Transform");
	ImGui::SameLine();

	if (transform->myComponentIsOpen)
	{
		if (ImGui::ArrowButton("transformButton", ImGuiDir_Up))
		{
			transform->myComponentIsOpen = false;
		}
	}
	else
	{
		if (ImGui::ArrowButton("transformButton", ImGuiDir_Down))
		{
			transform->myComponentIsOpen = true;
		}
	}

	if (transform->myComponentIsOpen)
	{
		transform->RenderImGUI();
	}

	for (int i = 0; i < myComponents.size(); i++)
	{
		std::stringstream enabledStream;

		enabledStream << "##ComponentEnabled" << i;

		ImGui::Checkbox(enabledStream.str().c_str(), &myComponents[i]->enabled);
		ImGui::SameLine();

		if (myComponents[i]->myName)
		{
			ImGui::Text(myComponents[i]->myName);
			ImGui::SameLine();
		}
		
		if (ImGui::IsMouseReleased(1) && ImGui::IsItemHovered())
		{
			ImGui::OpenPopup("contextMenu");
			componentToBeDeleted = i;
		}

		if (myComponents[i]->myName)
		{
			if (myComponents[i]->myComponentIsOpen)
			{
				if (ImGui::ArrowButton(myComponents[i]->myName, ImGuiDir_Up))
				{
					myComponents[i]->myComponentIsOpen = false;
				}
			}
			else
			{
				if (ImGui::ArrowButton(myComponents[i]->myName, ImGuiDir_Down))
				{
					myComponents[i]->myComponentIsOpen = true;
				}
			}
		}
		

		if (myComponents[i]->myComponentIsOpen)
		{
			myComponents[i]->RenderImGUI();
		}
	}

	if (ImGui::BeginPopup("contextMenu"))
	{
		if (ImGui::Button("Delete Component"))
		{
			RemoveComponent(componentToBeDeleted);

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void GameObject::RemoveComponent(const int i)
{
	myComponents[i]->myToBeDeleted = true;

	for (int j = i; j < myComponents.size() - 1; j++)
	{
		myComponents[j] = myComponents[j + 1];
	}

	myComponents.pop_back();
}

std::vector<Component*>& GameObject::GetComponents()
{
	return myComponents;
}

void GameObject::SetName(std::string aName)
{
	std::shared_ptr<GameObject> go = std::move(MainSingleton::Get()->activeScene->gameObjects.at(myName));
	MainSingleton::Get()->activeScene->gameObjects.erase(myName);

	MainSingleton::Get()->activeScene->gameObjects.emplace(go->myName, go);

	myName = aName;
}

const std::string& GameObject::GetName()
{
	return myName;
}

std::vector<std::shared_ptr<GameObject>>& GameObject::GetChildren()
{
	return myChildren;
}

std::shared_ptr<GameObject> GameObject::GetChild(const int aIndex)
{
	return myChildren[aIndex];
}

void GameObject::PushNewChild(std::shared_ptr<GameObject> aChild)
{
	aChild->myParent = this;

	myChildren.push_back(aChild);
}

GameObject* GameObject::GetParent() const
{
	return myParent;
}

void GameObject::SetParent(GameObject* aParent)
{
	myParent = aParent;

	//	aParent->myChildren.push_back(std::make_shared<GameObject>(this));
}
