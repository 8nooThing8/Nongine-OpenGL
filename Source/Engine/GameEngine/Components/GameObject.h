#pragma once

#include <memory>
#include <vector>

#include "../Components/Component.h"
#include "../ComponentManager.h"
#include "Transform.h"
#include "../MainSingleton.h"

class GameObject
{
public:

	GameObject();

	GameObject(const std::string& aName);

	~GameObject();

	std::shared_ptr<GameObject> DuplicateObject();

	void SetActive(bool aActive);

	bool& GetActive();

	void RenderImGUI();

	void StartComponents();

	template<class Comp>
	void RemoveComponent();

	void RemoveComponent(const int i);

	void AddComponent(Component* aComponent, const std::string& aName);

	template<class Comp>
	Comp* AddComponent(const bool addToComponentManager = true);

	template<class Comp>
	Comp* GetComponent();

	template<class Comp>
	Comp* GetComponent(bool base);

	std::vector<Component*>& GetComponents();

	template <class T>
	std::vector<T*> GetComponents();

	void SetName(std::string aName);

	void SetID(int aID);

	const std::string& GetName();
	uint32_t GetID();
	int GetRawID();

	std::vector<std::shared_ptr<GameObject>>& GetChildren();

	std::shared_ptr<GameObject> GetChild(const int aIndex);

	void PushNewChild(std::shared_ptr<GameObject> aChild);

	GameObject* GetParent() const;

	void SetParent(GameObject* aParent);

	static void Destroy(std::shared_ptr<GameObject>& aGameobject);

	void CallOnCollisionEnter(GameObject* aGO, const CU::Vector3f* aContactPoint);
	void CallOnCollisionExit(GameObject* aGO);
private:
	std::string myName;

	GameObject* myParent;
	std::vector<std::shared_ptr<GameObject>> myChildren;

	std::vector<Component*> myComponents;

	bool myIsActive = true;
	int myID;
public:
	Transform* transform;

	static inline int nextID = 0;

	bool open = false;

	int componentToBeDeleted;

	bool myToBeDeleted;
};

template <class Comp>
inline void GameObject::RemoveComponent()
{
	if (typeid(Comp) == typeid(Transform))
	{
		assert(!"One can not delete transform component");
	}

	for (int i = 0; i < myComponents.size(); i++)
	{
		if (typeid(*myComponents[i]) == typeid(Comp))
		{
			myComponents[i]->myToBeDeleted = true;

			for (int j = i; j < myComponents.size() - 1; j++)
			{
				myComponents[j] = myComponents[j + 1];
			}

			myComponents.pop_back();
		}
	}
}

inline void GameObject::AddComponent(Component* aComponent, const std::string& aName)
{
	aComponent->gameObject = this;

	aComponent->myName = aName;

	myComponents.emplace_back(aComponent);

	MainSingleton::Get()->myComponentManager->myComponents[aName].emplace_back(reinterpret_cast<Component*>(myComponents[myComponents.size() - 1]));

	aComponent->AddedAsComponent();
}

template <class Comp>
inline Comp* GameObject::AddComponent(const bool addToComponentManager)
{
	Comp* newComponent = new Comp();

	newComponent->gameObject = this;

	const std::type_info& type = typeid(Comp);

	newComponent->myName = std::strstr(type.name(), " ") + 1;


	myComponents.emplace_back(reinterpret_cast<Component*>(newComponent));

	if (addToComponentManager)
	{
		MainSingleton::Get()->myComponentManager->myComponents[newComponent->myName].emplace_back(reinterpret_cast<Component*>(myComponents[myComponents.size() - 1]));
	}

	newComponent->AddedAsComponent();

	return newComponent;
}

template<class Comp>
inline Comp* GameObject::GetComponent(bool /*base*/)
{
	if (typeid(Comp) == typeid(Transform))
	{
		assert(!"Use GetTransform() instead of GetComponent<Transform>()");
	}

	for (auto& component : myComponents)
	{
		auto casted = dynamic_cast<Comp*>(component);
		if (casted)
			return casted;

	}
	return nullptr;
}

template <class Comp>
inline Comp* GameObject::GetComponent()
{
	if (typeid(Comp) == typeid(Transform))
	{
		assert(!"Use transfom instead of GetComponent<Transform>()");
	}

	for (auto& component : myComponents)
	{
		if (typeid(*component) == typeid(Comp))
		{
			return reinterpret_cast<Comp*>(component);
		}
	}

	return nullptr;
}