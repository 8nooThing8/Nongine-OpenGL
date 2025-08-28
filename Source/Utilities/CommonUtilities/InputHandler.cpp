//#include "pch.h"

#include "InputHandler.h"
#include "Keycodes.h"

#include <assert.h>

#include <algorithm>

InputHandler* InputHandler::Instance = nullptr;

IVector2::IVector2(int aX, int aY)
{
	x = aX;
	y = aY;
}

IVector3::IVector3(int aX, int aY, int aZ)
{
	x = aX;
	y = aY;
	z = aZ;
}

InputHandler::InputHandler(/*const HWND& ahWnd*/)
{
	myCurrState = std::array<bool, 256>();
	myPrevState = std::array<bool, 256>();

	// myHWnd = ahWnd;

	myPrevMousePos = IVector2(0, 0);
	//myMousePoint = POINT();

	for (int i = 0; i < myCurrState.size(); i++)
	{
		myCurrState[i] = false;
		myPrevState[i] = false;
	}
}

void InputHandler::UpdateEvents(/* UINT aMessage, WPARAM aWParams, LPARAM aLParams */)
{
	/* switch (aMessage)
	{
	case WM_KEYUP:
		myPrevState = myCurrState;
		myCurrState[aWParams] = false;
		break;

	case WM_MBUTTONUP:
		myPrevState = myCurrState;
		myCurrState[keycode::MBUTTON] = false;
		break;

	case WM_LBUTTONUP:
		myPrevState = myCurrState;
		myCurrState[keycode::MOUSELBUTTON] = false;
		break;

	case WM_RBUTTONUP:
		myPrevState = myCurrState;
		myCurrState[keycode::MOUSERBUTTON] = false;
		break;

	case WM_MOUSEMOVE:
	{
		const int xPos = GET_X_LPARAM(aLParams);
		const int yPos = GET_Y_LPARAM(aLParams);

		myTentativeMousePosition.x = xPos;
		myTentativeMousePosition.y = yPos;

		break;
	}
	case WM_MBUTTONDOWN:
		myPrevState = myCurrState;
		myCurrState[keycode::MBUTTON] = true;
		break;

	case WM_LBUTTONDOWN:
		myPrevState = myCurrState;
		myCurrState[keycode::MOUSELBUTTON] = true;
		break;

	case WM_RBUTTONDOWN:
		myPrevState = myCurrState;
		myCurrState[keycode::MOUSERBUTTON] = true;
		break;

	case WM_KEYDOWN:
		myPrevState = myCurrState;
		myCurrState[aWParams] = true;
		break;
	case WM_MOUSEHWHEEL:
	case WM_MOUSEWHEEL:
	{
		int scrollDelta = GET_WHEEL_DELTA_WPARAM(aWParams);
		myScrollWheelDeltaZ += static_cast<float>(scrollDelta);
	}

	} */

	return;
}

void InputHandler::SetMousePositionNormalized(float aXPos, float aYPos)
{
	assert((aXPos <= 1 && aXPos >= 0) && "Mouse is outside of range. Must be 0 - 1.");
	assert((aYPos <= 1 && aYPos >= 0) && "Mouse is outside of range. Must be 0 - 1.");

	/* RECT rect;
	GetClientRect(myHWnd, &rect);

	int x = static_cast<int>(aXPos * rect.right);
	int y = static_cast<int>(aYPos * rect.bottom);

	POINT targetPos = { static_cast<LONG>(rect.left + x), static_cast<LONG>(rect.top + y) };

	ClientToScreen(myHWnd, &targetPos);

	SetCursorPos(targetPos.x, targetPos.y); */
}

void InputHandler::SetMousePosition(int /* aXPos */, int /* aYPos */)
{
	/* POINT targetPos = { static_cast<LONG>(aXPos), static_cast<LONG>(aYPos) };

	ClientToScreen(myHWnd, &targetPos);

	SetCursorPos(targetPos.x, targetPos.y); */
}

void InputHandler::ShowMyCursor(const bool aIsVisible)
{
	myIsCursorVisible = aIsVisible;

/* 	if (aIsVisible)
	{
		ShowCursor(aIsVisible);
	}
	else
	{
		SetCursor(nullptr);
	} */
}

void InputHandler::SetMousePosBounds(IVector2 aWindowMin, IVector2 aWindowMax)
{
	boundsMousePosMin = aWindowMin;
	boundsMousePosMax = aWindowMax;
}

bool InputHandler::GetKeyDown(const int aKeyCode)
{
	InputHandler* instance = InputHandler::GetInstance();

	bool returnValue = !instance->myPrevState[aKeyCode] && instance->myCurrState[aKeyCode];

	if (returnValue)
	{
		instance->myPrevState[aKeyCode] = instance->myCurrState[aKeyCode];
	}

	return returnValue;
}
bool InputHandler::GetKeyUp(const int aKeyCode)
{
	InputHandler* instance = InputHandler::GetInstance();

	bool returnValue = instance->myPrevState[aKeyCode] && !instance->myCurrState[aKeyCode];

	if (!returnValue)
	{
		instance->myPrevState[aKeyCode] = false;
	}

	return returnValue;
}
bool InputHandler::GetKey(const int aKeyCode)
{
	return InputHandler::GetInstance()->myCurrState[aKeyCode];
}

bool InputHandler::GetMouseButtonDown(const int aMouseButtonIndex)
{
	bool returnvalue;

	InputHandler* instance = InputHandler::GetInstance();

	switch (aMouseButtonIndex)
	{
	case 0:
		returnvalue = instance->myPrevState[keycode::MOUSELBUTTON] == false && instance->myCurrState[keycode::MOUSELBUTTON] == true;

		instance->myPrevState[keycode::MOUSELBUTTON] = instance->myCurrState[keycode::MOUSELBUTTON];

		return returnvalue;
	case 1:
		returnvalue = instance->myPrevState[keycode::MOUSERBUTTON] == false && instance->myCurrState[keycode::MOUSERBUTTON] == true;

		instance->myPrevState[keycode::MOUSERBUTTON] = instance->myCurrState[keycode::MOUSERBUTTON];

		return returnvalue;
	case 2:
		returnvalue = instance->myPrevState[keycode::MBUTTON] == false && instance->myCurrState[keycode::MBUTTON] == true;

		instance->myPrevState[keycode::MBUTTON] = instance->myCurrState[keycode::MBUTTON];

		return returnvalue;
	}

	return false;
}

bool InputHandler::GetMouseButton(const int aMouseButtonIndex)
{
	

	InputHandler* instance = InputHandler::GetInstance();

	switch (aMouseButtonIndex)
	{
	case 0:
		return instance->myCurrState[keycode::MOUSELBUTTON];
	case 1:
		return instance->myCurrState[keycode::MOUSERBUTTON];
	case 2:
		return instance->myCurrState[keycode::MBUTTON];
	}

	return false;
}

// POINT InputHandler::GetMousePos()
// {
// 	return myMousePoint;
// }

// POINT InputHandler::GetRelativeMousePos()
// {
// 	return myTentativeMousePosition;
// }

IVector2 InputHandler::GetMouseDelta()
{
	return myDeltaPos;
}

float InputHandler::GetScrollWheelDelta()
{
	return myScrollWheelDeltaZ;
}

void InputHandler::SetMouseDeltaZero()
{
	myScrollWheelDeltaZ = 0;
}

float mapValue(int value, int inMin, int inMax, int outMin, int outMax)
{
	return (static_cast<float>(value) - static_cast<float>(inMin)) * (static_cast<float>(outMax) - static_cast<float>(outMin)) / (static_cast<float>(inMax) - static_cast<float>(inMin)) + static_cast<float>(outMin);
}

int clamp(float value, int minValue, int maxValue) {
	if (value < minValue)
	{
		return minValue;
	}
	if (value > maxValue)
	{
		return maxValue;
	}
	return static_cast<int>(value);
}

void InputHandler::UpdateInput()
{
	/* HMONITOR monitor = MonitorFromWindow(myHWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO info;
	info.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(monitor, &info);

	RECT rect;

	GetWindowRect(myHWnd, &rect);

	//int width = rect.right - rect.left;
	//int height = rect.bottom - rect.top;

	boundsMousePosMin = IVector2(rect.left, rect.top);
	boundsMousePosMax = IVector2(rect.right, rect.bottom);

	if (!myMouseConfinedInCenter)
	{
		myPrevMousePos = IVector2(myMousePoint.x, myMousePoint.y);

		GetCursorPos(&myMousePoint);

		myDeltaPos = IVector2(myMousePoint.x - myPrevMousePos.x, myPrevMousePos.y - myMousePoint.y);
	}
	else
	{
		GetCursorPos(&myMousePoint);

		// Calculate the center of the screen
		int x = static_cast<int>(0.5f * rect.right);
		int y = static_cast<int>(0.5f * rect.bottom);

		// Reset the mouse position to the center
		SetCursorPos(x, y);

		// Calculate the delta based on the movement before the reset
		myDeltaPos = IVector2(myMousePoint.x - x, y - myMousePoint.y);

		// Update the previous mouse position to the new center position
		myPrevMousePos = IVector2(x, y);
	}

 */
}

keycode::Keys InputHandler::GetAnyKey()
{
	for (int i = 0; i < myCurrState.size(); i++)
	{
		if (myCurrState[i])
		{
			return static_cast<keycode::Keys>(i);
		}
	}

	return static_cast<keycode::Keys>(0);
}

void InputHandler::ChangeMouseLockMode(const MouseLockMode& aMouseLockMode)
{
	/* switch (aMouseLockMode)
	{
	case MouseLockMode::Restricted:
	{
		RECT r;
		GetWindowRect(myHWnd, &r);
		ClipCursor(&r);
		break;
	}
	case MouseLockMode::Unrestricted:
	{
		ClipCursor(NULL);
		break;
	}
	case MouseLockMode::Confined:
	{
		myMouseConfinedInCenter = true;
		break;
	}
	case MouseLockMode::Unconfined:
	{
		myMouseConfinedInCenter = false;
		break;
	}
	default:
	{
		assert(false && "Invalid lock mode.");
		break;
	}
	} */
}