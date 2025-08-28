
#include <ImGui/imgui.h>

#include "GameObject.h"

#include "Transform.h"
#include "Component.h"

#include "TypeRegistry.h"

#include <Hash.h>

#undef max

void GameObject::GameobjectConstructor()
{
	myID = nextID++;

	CU::Vector3i colorInt;

	colorInt.x = (myID & 0x000000FF) >> 0;
	colorInt.y = (myID & 0x0000FF00) >> 8;
	colorInt.z = (myID & 0x00FF0000) >> 16;

	myPixelPickRGB = static_cast<CU::Vector3f>(colorInt) / 255.f;

	Transform* tf = new Transform();
	tf->gameObject = this;
	transform = tf;

	myToBeDeleted = false;
}

GameObject::GameObject() : myName("NoNameObject")
{
	GameobjectConstructor();
}

GameObject::GameObject(const std::string& aName) : myName(aName)
{
	GameobjectConstructor();
}

GameObject::~GameObject()
{
	for (auto& component : myComponents)
	{
		component->myToBeDeleted = true;
	}
}

void GameObject::Destroy(GameObject*& aGameobject)
{
	aGameobject->myToBeDeleted = true;
}

void GameObject::Destroy()
{
	myToBeDeleted = true;
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

void GameObject::StartMyComponents()
{
	for (auto comp : myComponents)
	{
		comp->Start();
	}
}

std::vector<Component*>& GameObject::GetComponents()
{
	return myComponents;
}

void GameObject::RunOnTransform()
{
	for (auto comp : myComponents)
	{
		comp->OnTransform();
	}
}

template <class T>
std::vector<T*> GameObject::GetComponents()
{
	std::vector<T*> comps;

	for (auto comp : myComponents)
	{
		auto component = dynamic_cast<T*>(comp);

		if (component)
		{
			comps.emplace_back(component);
		}
	}

	return comps;
}

void GameObject::SetName(std::string aName)
{
	myName = aName;
}

const std::string& GameObject::GetName()
{
	return myName;
}

void GameObject::SetID(int aID)
{
	myID = aID;

	nextID = std::max(nextID + 1, static_cast<unsigned>(aID));
}

int GameObject::GetRawID()
{
	return myID;
}

uint32_t GameObject::GetID()
{
	return CU::Hash(myID);
}

void GameObject::CallOnCollisionEnter(GameObject* aGO, const CU::Vector3f* aContactPoint)
{
	for (auto& component : myComponents)
	{
		component->OnCollisionEnter(aGO, aContactPoint);
	}
}

void GameObject::CallOnCollisionExit(GameObject* aGO)
{
	for (auto& component : myComponents)
	{
		component->OnCollisionExit(aGO);
	}
}

const CU::Vector3f& GameObject::GetPixelPickRGB()
{
	return myPixelPickRGB;
}

bool GameObject::GetToBeDeleted()
{
	return myToBeDeleted;
}
