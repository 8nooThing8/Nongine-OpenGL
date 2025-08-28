#pragma once

#include <memory>
#include <vector>

#include "../Components/Component.h"
#include "../ComponentManager.h"
#include "Transform.h"

#include "cstring"

#include "../Engine/GameEngine/MainSingleton.h"

class GameObject
{
public:
	inline void GameobjectConstructor();
	GameObject();
	GameObject(const std::string &aName);
	~GameObject();

	template <class Comp>
	void RemoveComponent();
	void RemoveComponent(const int i);

	template <class Comp, class... Args>
	Comp *AddComponent(Args... aArgs);
	void AddComponent(Component *aComponent, const std::string &aName);

	template <class Comp>
	Comp *GetComponent();
	template <class Comp>
	Comp *GetComponent(bool base) const;

	void StartMyComponents();

	template <class T>
	std::vector<T *> GetComponents();
	std::vector<Component *> &GetComponents();

	void RunOnTransform();

	void SetName(std::string aName);
	void SetID(int aID);

	const std::string &GetName();
	int GetRawID();
	uint32_t GetID();

	static void Destroy(GameObject *&aGameobject);
	void Destroy();

	void CallOnCollisionEnter(GameObject *aGO, const CU::Vector3f *aContactPoint);
	void CallOnCollisionExit(GameObject *aGO);

	const CU::Vector3f &GetPixelPickRGB();

	bool GetToBeDeleted();

public:
	Transform *transform;
	static inline unsigned nextID = 1;

	static inline unsigned intDiv2 = 429496729 / 2;

	int myPartOfLobby = -1;
private:

	std::string myName;
	std::vector<Component *> myComponents;
	int myID;

	CU::Vector3f myPixelPickRGB;
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

inline void GameObject::AddComponent(Component *aComponent, const std::string &aName)
{
	aComponent->gameObject = this;

	std::memcpy(aComponent->myName, aName.c_str(), aName.length());

	myComponents.emplace_back(aComponent);

	MainSingleton::Get().myComponentManager->myComponents[aName.c_str()].emplace_back(reinterpret_cast<Component *>(myComponents[myComponents.size() - 1]));

	aComponent->AddedAsComponent();
}

template <class Comp, class... Args>
inline Comp *GameObject::AddComponent(Args... aArgs)
{
	Comp *newComponent = new Comp(std::forward<Args>(aArgs)...);

	newComponent->gameObject = this;

	const std::type_info &type = typeid(Comp);

	const char *str = nullptr;

#ifdef NONWIN32 
	str = std::strstr(type.name(), " ") + 1;
#elif NONEMSCRIPTEN
	str = std::strstr(type.name(), " ") + 1;
#elif NONUNIX
	int status = 0;
	const char *mangled = type.name();
	str = abi::__cxa_demangle(mangled, nullptr, nullptr, &status);
#endif

	std::memcpy(newComponent->myName, str, strlen(str));

	myComponents.emplace_back(reinterpret_cast<Component *>(newComponent));

	MainSingleton::Get().myComponentManager->myComponents[newComponent->myName].emplace_back(reinterpret_cast<Component *>(myComponents[myComponents.size() - 1]));

	newComponent->AddedAsComponent();

	return newComponent;
}

template <class Comp>
inline Comp *GameObject::GetComponent(bool /*base*/) const
{
	assert(typeid(Comp) != typeid(Transform) && "Use GetTransform() instead of GetComponent<Transform>()");

	for (auto &component : myComponents)
		if (auto casted = dynamic_cast<Comp *>(component))
			return casted;
	return nullptr;
}

template <class Comp>
inline Comp *GameObject::GetComponent()
{
	assert(typeid(Comp) != typeid(Transform) && "Use transfom instead of GetComponent<Transform>()");

	for (auto &component : myComponents)
		if (typeid(*component) == typeid(Comp))
			return reinterpret_cast<Comp *>(component);

	return nullptr;
}