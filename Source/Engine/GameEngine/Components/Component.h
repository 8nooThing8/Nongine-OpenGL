#pragma once

#include <Vector/Vector3.hpp>

class GameObject;

class Component
{
public:
	Component() : myComponentIsOpen(false), myToBeDeleted(false)
	{
		myID = NextID++;
	}

	Component(Component& /*aComp*/) : myComponentIsOpen(false), myToBeDeleted(false)
	{
		Component();
	}

	virtual ~Component() = default;

	virtual void OnTransform()
	{

	}

	virtual void AddedAsComponent()
	{

	}

	virtual void Start()
	{
		
	}
	
	virtual void Stop()
	{

	}

	virtual void Update(float /*aDeltaTime*/)
	{

	}

	virtual void UpdateEditor(float /*aDeltaTime*/)
	{

	}

	virtual void EarlyUpdate(float /*aDeltaTime*/)
	{

	}

	virtual void EarlyUpdateEditor(float /*aDeltaTime*/)
	{

	}

	virtual void LateUpdate(float /*aDeltaTime*/)
	{

	}

	virtual void LateUpdateEditor(float /*aDeltaTime*/)
	{

	}

	virtual void RenderImGUI()
	{
		
	}

	//virtual void HandleSave(rapidjson::Value& /*aOutJson*/, rapidjson::Document& /*aDocument*/)
	//{

	//}
	//virtual void HandleLoad(const rapidjson::Value& /*aObj*/)
	//{

	//}

	virtual void OnCollisionEnter(GameObject* /*aGo*/, const CommonUtilities::Vector3f* /*aContactPoint*/)
	{

	}
	virtual void OnCollisionExit(GameObject* /*aGo*/)
	{

	}

public:
	static inline int NextID = 0;
	int myID;

	GameObject* gameObject = nullptr;

	char myName[256] = "";

	bool myComponentIsOpen;

	bool myToBeDeleted;

	bool enabled = true;
};
