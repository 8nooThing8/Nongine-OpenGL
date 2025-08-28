#include "GraphicsCommandList.h"

#include "../Engine/GameEngine/Components/GameObject.h"

#include <algorithm>

GraphicsCommandListIterator::GraphicsCommandListIterator(const GraphicsCommandList* aCommandList)
{
	if (aCommandList->HasCommands())
		myPtr = aCommandList->myRoot;
}

GraphicsCommandBase* GraphicsCommandListIterator::Next()
{
	GraphicsCommandBase* cmd = myPtr;
	myPtr = cmd->Next;
	return cmd;
}

GraphicsCommandList::GraphicsCommandList(const size_t aMemorySize) :
	mySize(aMemorySize),
	myData(new uint8_t[mySize]),
	myRoot(reinterpret_cast<GraphicsCommandBase*>(myData)),
	myLink(&myRoot)
{
	memset(myData, 0, mySize);
}

GraphicsCommandList::~GraphicsCommandList()
{
	Reset();

	delete[] myData;
	myData = nullptr;
	myRoot = nullptr;
	myLink = nullptr;
}

void GraphicsCommandList::Execute()
{
	if (!isExecuting && !isfinished)
	{
		isExecuting = true;
		GraphicsCommandListIterator it(this);

		while (it)
		{
			GraphicsCommandBase* cmd = it.Next();
			cmd->Execute();
			cmd->Destroy();
		}

		isfinished = true;
		isExecuting = false;
	}
}

void GraphicsCommandList::ExecuteWithoutDestroying()
{
	if (!isExecuting && !isfinished)
	{
		isExecuting = true;
		GraphicsCommandListIterator it(this);

		while (it)
		{
			GraphicsCommandBase* cmd = it.Next();
			cmd->Execute();
		}

		isfinished = false;
		isExecuting = false;

		myRoot = reinterpret_cast<GraphicsCommandBase*>(myData);
		myLink = &myRoot;
	}
}

void GraphicsCommandList::Reset()
{
	if (isExecuting)
	{
		throw std::runtime_error("Cannot reset command list whilst it is executing");
	}

	if (myNumCommands > 0)
	{
		if (!isfinished)
		{
			GraphicsCommandListIterator it(this);

			while (it)
			{
				GraphicsCommandBase* cmd = it.Next();
				cmd->Destroy();
			}

		}

		memset(myData, 0, mySize);
		myRoot = reinterpret_cast<GraphicsCommandBase*>(myData);
		myLink = &myRoot;
		myCursor = 0;
		myNumCommands = 0;
	}

	isExecuting = false;
	isfinished = false;
}
