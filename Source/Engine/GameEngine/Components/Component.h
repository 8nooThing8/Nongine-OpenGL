#pragma once

#include "rapidjson-master/include/rapidjson/document.h"

class GameObject;
struct NetworkRecieveData;

class Component
{
public:
	Component()
	{
		myID = NextID++;
	}

	Component(Component& /*aComp*/) : myComponentIsOpen(false), myToBeDeleted(false)
	{
		myID = NextID++;
	}

	virtual ~Component() = default;

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

	virtual void HandleSave(rapidjson::Value& /*aOutJson*/, rapidjson::Document& /*aDocument*/)
	{

	}
	virtual void HandleLoad(const rapidjson::Value& /*aObj*/)
	{

	}

	virtual void OnCollisionEnter(GameObject* aGo, const CU::Vector3f* aContactPoint)
	{
		aGo;
		aContactPoint;
	}
	virtual void OnCollisionExit(GameObject* aGo)
	{
		aGo;
	}

public:
	static inline int NextID = 0;
	int myID;

	GameObject* gameObject = nullptr;

	std::string myName = "";

	bool myComponentIsOpen;

	bool myToBeDeleted;

	bool enabled = true;
};