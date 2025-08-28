#pragma once

#include "Matrix3x3.hpp"
#include "Matrix4x4.hpp"

namespace CommonUtilities
{
	template <typename T>
	inline Matrix3x3<T>::Matrix3x3(const Matrix4x4<T>& aMatrix)
	{
		std::memcpy(myData, aMatrix.myData, 12);
		std::memcpy(myData + 3, aMatrix.myData + 4, 12);
		std::memcpy(myData + 6, aMatrix.myData + 8, 12);
	}
}
