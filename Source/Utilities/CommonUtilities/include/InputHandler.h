#pragma once

#define WIN32_LEAN_AND_MEAN
//#include <Windows.h>
#include <array>

#include "Keycodes.h"

#define SETUP_INPUT_HANDLER InputHandler::GetInstance()->Init();

struct IVector2
{
	int x;
	int y;

	IVector2() = default;

	IVector2(int aX, int aY);
};

struct IVector3
{
	int x;
	int y;
	int z;

	IVector3() = default;

	IVector3(int aX, int aY, int aZ);
};

class InputHandler
{
public:
	enum MouseLockMode
	{
		Restricted,
		Unrestricted,
		Confined,
		Unconfined
	};

	InputHandler(/*const HWND& ahWnd*/);

	//InputHandler() = default;

	~InputHandler() = default;

	void UpdateEvents(/*UINT aMessage, WPARAM aWParams, LPARAM aLParams*/);

	static bool GetKeyDown(const int aKeyCode);
	static bool GetKeyUp(const int aKeyCode);
	static bool GetKey(const int aKeyCode);
	static bool GetMouseButtonDown(const int aMouseButtonIndex);
	static bool GetMouseButton(const int aMouseButtonIndex);
	

	//POINT GetMousePos();
	//POINT GetRelativeMousePos();

	IVector2 GetMouseDelta();

	float GetScrollWheelDelta();

	void SetMouseDeltaZero();

	void UpdateInput();

	void ChangeMouseLockMode(const MouseLockMode& aMouseLockMode);

	void SetMousePositionNormalized(float aXPos, float aYPos);
	void SetMousePosition(int aXPos, int aYPos);

	void ShowMyCursor(const bool aIsVisible);

	void SetMousePosBounds(IVector2 aWindowMin, IVector2 aWindowMax);

	void SetHWND(/*HWND& aHWND*/) { /*myHWnd = aHWND;*/ };

	keycode::Keys GetAnyKey();

	void Init()
	{
		if (!Instance)
			Instance = new InputHandler();
	}

	static inline InputHandler* GetInstance()
	{
		return Instance;
	}

private:
	// HWND myHWnd;
	float myScrollWheelDeltaZ;

	std::array<bool, 256> myCurrState;
	std::array<bool, 256> myPrevState;

	IVector2 boundsMousePosMin;
	IVector2 boundsMousePosMax;

	//POINT myMousePoint;
	//POINT myTentativeMousePosition;
	IVector2 myPrevMousePos;
	IVector2 myDeltaPos;


	bool myMouseConfinedInCenter = false;

	static InputHandler* Instance;

	bool myIsCursorVisible;
};