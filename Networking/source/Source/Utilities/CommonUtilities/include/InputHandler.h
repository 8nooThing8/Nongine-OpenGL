#pragma once

#include <Windows.h>
#include <array>

#include "Keycodes.h"

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

	InputHandler(const HWND& ahWnd);

	InputHandler() = default;

	~InputHandler() = default;

	void UpdateEvents(UINT aMessage, WPARAM aWParams, LPARAM aLParams);

	bool GetKeyDown(const int aKeyCode);

	bool GetKeyUp(const int aKeyCode);

	bool GetKey(const int aKeyCode) const;

	bool GetMouseButtonDown(const int aMouseButtonIndex);

	bool GetMouseButton(const int aMouseButtonIndex);

	POINT GetMousePos();
	POINT GetRelativeMousePos();

	IVector2 GetMouseDelta();

	float GetScrollWheelDelta();

	void SetMouseDeltaZero();

	bool MousePressed(int aIndex) const;

	void UpdateInput();

	void ChangeMouseLockMode(const MouseLockMode& aMouseLockMode);

	void SetMousePositionNormalized(float aXPos, float aYPos);
	void SetMousePosition(int aXPos, int aYPos);

	void ShowMyCursor(const bool aIsVisible);

	void SetMousePosBounds(IVector2 aWindowMin, IVector2 aWindowMax);

	keycode::Keys GetAnyKey();

	static inline InputHandler* GetInstance()
	{
		if (!Instance)
			Instance = new InputHandler();

		return Instance;
	}

	HWND myHWnd;
private:
	float myScrollWheelDeltaZ;

	std::array<bool, 256> myCurrState;
	std::array<bool, 256> myPrevState;

	IVector2 boundsMousePosMin;
	IVector2 boundsMousePosMax;

	POINT myMousePoint;
	POINT myTentativeMousePosition;
	IVector2 myPrevMousePos;
	IVector2 myDeltaPos;


	bool myMouseConfinedInCenter = false;

	static InputHandler* Instance;

	bool myIsCursorVisible;
};