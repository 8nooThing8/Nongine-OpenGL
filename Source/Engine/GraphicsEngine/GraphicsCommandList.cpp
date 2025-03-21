#include "../Engine.pch.h"
#include "GraphicsCommandList.h"

#include "../GraphicsEngine/RenderHardwareInterface.h"
#include "../GameEngine/Components/GameObject.h"

#include <Vector/Vector4.hpp>

#include "../GameEngine/InspectorCamera.h"

#include "../GameEngine/Camera.h"

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
	//float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	//UINT sampleMask = 0xffffffff;

	//RenderHardwareInterface::myContext->OMSetBlendState(GraphicsEngine::Get().myBlendstate.Get(), 0, sampleMask);

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

void GraphicsCommandList::ExecuteWithoutDestroy()
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

		isfinished = true;
		isExecuting = false;
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
		myObjects.clear();
	}

	isExecuting = false;
	isfinished = false;
}
