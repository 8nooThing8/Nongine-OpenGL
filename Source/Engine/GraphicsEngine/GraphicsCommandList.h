#pragma once
#include <functional>

#include <stdexcept>

#include <vector>

class GameObject;

struct GraphicsCommandBase
{
	virtual void Execute() = 0;
	virtual void Destroy() = 0;

	GraphicsCommandBase* Next = nullptr;
};

using GraphicsCommandFunction = std::function<void()>;

struct LambdaGraphicsCommand final : public GraphicsCommandBase
{
	GraphicsCommandFunction myLamda;

	LambdaGraphicsCommand(GraphicsCommandFunction&& aLambda) : myLamda(std::move(aLambda))
	{

	}

	void Execute() override
	{
		myLamda();
	}

	void Destroy() override
	{
		myLamda.~GraphicsCommandFunction();
	}
};

class GraphicsCommandList
{

public:

	// Add more for more commands
	GraphicsCommandList(size_t aMemorySize = 1048576);
	~GraphicsCommandList();

	GraphicsCommandList(const GraphicsCommandList&) = delete;
	GraphicsCommandList(GraphicsCommandList&&) noexcept = delete;
	GraphicsCommandList& operator=(GraphicsCommandList&&) noexcept = delete;
	GraphicsCommandList& operator=(const GraphicsCommandList&&) = delete;

	void Execute();
	void ExecuteWithoutDestroy();
	void Reset();

	__forceinline bool IsFinished() const { return !isExecuting && isfinished; }
	__forceinline bool IsExecuting() const { return isExecuting; }
	__forceinline bool HasCommands() const { return myNumCommands != 0; }

	template <class CommandClass, class ...Args>
	std::enable_if_t<std::is_base_of_v<GraphicsCommandBase, CommandClass>> Enqueue(Args... args)
	{
		const size_t commandSize = sizeof(CommandClass);

		if (myCursor + commandSize > mySize)
		{
			throw std::out_of_range("graphics command list ran out of memory, increase it maybe");
		}

		GraphicsCommandBase* ptr = reinterpret_cast<GraphicsCommandBase*>(myData + myCursor);
		myCursor += commandSize;
		::new(ptr) CommandClass(std::forward<Args>(args)...);
		*myLink = ptr;
		myLink = &ptr->Next;
		myNumCommands++;
	}

	void Enqueue(GraphicsCommandFunction&& aLamba)
	{
		if (aLamba)
		{
			Enqueue<LambdaGraphicsCommand>(std::move(aLamba));
		}
	}

	std::vector<GameObject*> myObjects;

private:
	friend class GraphicsCommandListIterator;

	size_t mySize = 0;
	size_t myCursor = 0;
	size_t myNumCommands = 0;

	uint8_t* myData = nullptr;

	GraphicsCommandBase* myRoot = nullptr;
	GraphicsCommandBase** myLink = nullptr;

	bool isExecuting = false;
	bool isfinished = false;

};

class GraphicsCommandListIterator
{
public:
	GraphicsCommandListIterator(const GraphicsCommandList* aCommandList);

	GraphicsCommandBase* Next();

	__forceinline operator bool()
	{
		return !!myPtr;
	}

	GraphicsCommandBase* myPtr = nullptr;
private:
};