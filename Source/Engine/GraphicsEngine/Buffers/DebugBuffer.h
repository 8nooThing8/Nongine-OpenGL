#pragma once

#include <Vector/Vector.hpp>

struct DebugBuffer
{
	int toneMapUsage;

	int isMask;

	CommonUtilities::Vector2<int> trashDB;
};