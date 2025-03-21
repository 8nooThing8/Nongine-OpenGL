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

		if (aMax < aValue)
		{
			aValue = aMax;
		}
		else if (aValue < aMin)
		{
			aValue = aMin;
		}

		return aValue;
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
	void Swap(T& aDataOne, T& aDataTwo)
	{
		T tempData = aDataOne;

		aDataOne = aDataTwo;
		aDataTwo = tempData;
	}

	template <typename T>
	T LookAt2D(CommonUtilities::Vector2<T>& aTarget)
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
	T MapValue(T aRangeMin, T aRangeMax, T aValueMin, T aValueMax, T aValue)
	{
		float returnValue = ((aValue - aValueMin) * (aRangeMax - aRangeMin) / (aValueMax - aValueMin)) + aRangeMin;

		return returnValue;
	}

	// // Curently a failed attempt but still trying :)
	// template <typename T>
	// std::vector<CommonUtilities::Vector3<T>> QuickSort(std::vector<CommonUtilities::Vector3<T>>& anArrayToSort)
	// {
	// 	std::vector<CommonUtilities::Vector3<T>> lowSortedArray;
	// 	std::vector<CommonUtilities::Vector3<T>> HighSortedArray;
	//
	// 	int i = -1;
	//
	// 	T pivot = anArrayToSort[anArrayToSort.size() - 1].z;
	//
	// 	for (int j = 0; j < anArrayToSort.size(); j++)
	// 	{
	// 		if (anArrayToSort[j].z < pivot)
	// 		{
	// 			++i;
	//
	// 			CommonUtilities::Swap(anArrayToSort[i], anArrayToSort[j]);
	// 		}
	// 	}
	//
	// 	++i;
	//
	// 	CommonUtilities::Swap(anArrayToSort[i], anArrayToSort[anArrayToSort.size() - 1]);
	//
	// 	std::vector<Vector3<float>> lowerPart(anArrayToSort.begin(), anArrayToSort.end() - static_cast<int>(anArrayToSort.size() - i));
	// 	std::vector<Vector3<float>> higherPart(anArrayToSort.begin() + static_cast<int>(i), anArrayToSort.end());
	//
	// 	if (anArrayToSort.size() > 1)
	// 	{
	// 		lowSortedArray = std::vector<CommonUtilities::Vector3<T>>(QuickSort(lowerPart));
	//
	// 		HighSortedArray = std::vector<CommonUtilities::Vector3<T>>(QuickSort(higherPart));
	// 	}
	// 	else
	// 	{
	// 		lowSortedArray = lowerPart;
	//
	// 		HighSortedArray = higherPart;
	// 	}
	//
	// 	std::vector<CommonUtilities::Vector3<T>> sortedArrayMerged = std::vector<CommonUtilities::Vector3<T>>();
	//
	// 	std::merge(lowSortedArray.begin(), lowSortedArray.end(), HighSortedArray.begin(), HighSortedArray.end(), std::back_inserter(sortedArrayMerged));
	//
	//
	// 	return sortedArrayMerged;
	// }
}