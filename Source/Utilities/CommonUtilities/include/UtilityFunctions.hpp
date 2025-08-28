#pragma once

#include <assert.h>

#include <vector>

#include <Vector/Vector.hpp>

#include <algorithm>

#define PI 3.1415926535897932384626

namespace CommonUtilities
{
	template <typename T>
	T Max(const T aNumberOne, const T aNumberTwo)
	{
		if (aNumberOne != aNumberTwo)
		{
			return aNumberOne < aNumberTwo ? aNumberTwo : aNumberOne;
		}
		else
		{
			return aNumberOne;
		}
	}

	template <typename T>
	T Min(const T aNumberOne, const T aNumberTwo)
	{
		if (aNumberOne != aNumberTwo)
		{
			return aNumberOne > aNumberTwo ? aNumberTwo : aNumberOne;
		}
		else
		{
			return aNumberOne;
		}
	}

	template <typename T>
	T Abs(const T aNumber)
	{
		return aNumber < 0 ? -aNumber : aNumber;
	}

	template <typename T>

	T Clamp(T aValue, const T aMin, const T aMax)
	{
		assert(aMin <= aMax);

		aValue = aValue < aMin ? aMin : aValue;
		return aValue > aMax ? aMax : aValue;
	}

	template <typename T>
	T Lerp(T aMin, T aMax, float aValue)
	{
		return static_cast<T>((aMax - aMin) * aValue + aMin);
	}

	template <typename T>
	T CubicInterporlation(T aMin, T aMax, float aValue)
	{
		return static_cast<T>((aMax - aMin) * (3.f - aValue * 2.f) * aValue * aValue + aMin);
	}

	template <typename T>
	void Swap(T &aDataOne, T &aDataTwo)
	{
		T tempData = aDataOne;

		aDataOne = aDataTwo;
		aDataTwo = tempData;
	}

	template <typename T>
	T LookAt2D(CommonUtilities::Vector2<T> &aTarget)
	{
		return static_cast<float>(2 * std::atan(aTarget.y / (aTarget.x + std::sqrt(std::pow(aTarget.x, 2) + std::pow(aTarget.y, 2)))) + PI);
	}

	const float deg2rad = 0.01745329251f;
	const float rad2deg = 57.2957795131f;

	// Converts Radians to Degrees
	template <typename T>
	T RadToDeg(T rads)
	{
		return static_cast<T>(rad2deg * rads);
	}

	template <typename T>
	T DegToRad(T degrees)
	{
		return static_cast<T>(deg2rad * degrees);
	}

	template <typename T>
	T MapValue(T aInputMin, T aInputMax, T aOutputMin, T aOutputMax, T aValue)
	{
		if (aOutputMax - aOutputMin == 0)
			return 0;

		T returnValue = ((aValue - aOutputMin) * (aInputMax - aInputMin) / (aOutputMax - aOutputMin)) + aInputMin;

		return returnValue;
	}
}