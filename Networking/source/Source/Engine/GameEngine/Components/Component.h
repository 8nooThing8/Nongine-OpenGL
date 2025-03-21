#pragma once

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

	virtual void RecieveDataNetwork(NetworkRecieveData& /*aNetworkDataRecieve*/)
	{

	}

public:
	static inline int NextID = 0;
	int myID;

	GameObject* gameObject = nullptr;

	const char* myName = "";

	bool myComponentIsOpen;

	bool myToBeDeleted;

	bool enabled = true;
};