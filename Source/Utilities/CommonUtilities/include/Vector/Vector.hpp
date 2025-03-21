#pragma once

#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

#include "Quaternion\Quaternion.hpp"

namespace CommonUtilities
{
	template<class T>
	inline Quaternion Vector3<T>::ToQuarternion() const
	{
		float halfRoll = x * 0.5f;
		float halfPitch = y * 0.5f;
		float halfYaw = z * 0.5f;

		// Compute sine and cosine of half angles
		float sinRoll = sin(halfRoll);
		float cosRoll = cos(halfRoll);
		float sinPitch = sin(halfPitch);
		float cosPitch = cos(halfPitch);
		float sinYaw = sin(halfYaw);
		float cosYaw = cos(halfYaw);

		// Compute quaternion components
		Quaternion quaternion;
		quaternion.x = sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw;
		quaternion.y = cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw;
		quaternion.z = cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw;
		quaternion.w = cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw;

		quaternion.Normalize();

		return quaternion;
	}

	// no conversion
	template<class T>
	inline Quaternion Vector4<T>::ToQuat() const
	{
		return Quaternion(x, y, z, w);
	}

	template<class T>
	inline Vector2<T> Vector4<T>::ToVector2() const
	{
		return Vector2<T>(x, y);
	}

	template<class T>
	inline Vector3<T> Vector4<T>::ToVector3() const
	{
		return Vector3<T>(x, y, z);
	}

	//template<class T>
	//inline Vector4<T> Vector3<T>::ToVector4() const
	//{
	//	return Vector4<T>(x, y, z, 1);
	//}

	//template<class T>
	//inline Vector4<T> Vector3<T>::ToVector4(float aW) const
	//{
	//	return Vector4<T>(x, y, z, aW);
	//}
}