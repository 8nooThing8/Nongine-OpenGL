class EventSubscriber
{
public:
	EventSubscriber() = default;

	~EventSubscriber() = default;

	friend class PostMaster;

	FunctionPointer GetFunc()
	{
		return myFunc;
	}

	GameObject* GetGameObject()
	{
		return myGameObject;
	}

private:
	FunctionPointer myFunc;
	GameObject* myGameObject;
};