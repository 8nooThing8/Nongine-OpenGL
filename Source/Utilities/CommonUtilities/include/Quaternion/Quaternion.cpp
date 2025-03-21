//#include "pch.h"

#include "Quaternion.hpp"

#include <Vector\Vector4.hpp>
#include <Vector\Vector3.hpp>

#include <Matrix/Matrix4x4.hpp>

namespace CommonUtilities
{

	Quaternion Quaternion::CreateFromAxisAngle(Vector3<float> anAxis, float anAngle)
	{
		float factor = sinf(anAngle * 0.5f);
		float x = anAxis.x * factor;
		float y = anAxis.y * factor;
		float z = anAxis.z * factor;
		float w = cosf(anAngle * 0.5f);

		return Quaternion(x, y, z, w);
	}

	/*Quaternion Quaternion::LookRotation(const  Vector3<float>& forward, const  Vector3<float>& up)
	{
		Vector3f normalizedForward = forward.GetNormalized();

		Vector3f right = up.Cross(normalizedForward).GetNormalized();
		Vector3f newUp = normalizedForward.Cross(right);

		Vector3f rotationAxis = Vector3f::Forward().Cross(normalizedForward);
		float angle = acosf(Vector3f::Forward().Dot(normalizedForward));

		return Quaternion::CreateFromAxisAngle(rotationAxis, angle);
	}*/

	CommonUtilities::Quaternion::Quaternion(const Vector3<float>& aEuler)
	{
		// Calculate half angles
		float halfRoll = aEuler.x * 0.5f;
		float halfPitch = aEuler.y * 0.5f;
		float halfYaw = aEuler.z * 0.5f;

		// Compute sine and cosine of half angles
		float sinRoll = sin(halfRoll);
		float cosRoll = cos(halfRoll);
		float sinPitch = sin(halfPitch);
		float cosPitch = cos(halfPitch);
		float sinYaw = sin(halfYaw);
		float cosYaw = cos(halfYaw);

		x = sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw;
		y = cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw;
		z = cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw;
		w = cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw;

		Normalize();
	}

	void CommonUtilities::Quaternion::InitWithEuler(const Vector3<float>& aEuler)
	{
		// Calculate half angles
		float halfRoll = aEuler.x * 0.5f;
		float halfPitch = aEuler.y * 0.5f;
		float halfYaw = aEuler.z * 0.5f;

		// Compute sine and cosine of half angles
		float sinRoll = sin(halfRoll);
		float cosRoll = cos(halfRoll);
		float sinPitch = sin(halfPitch);
		float cosPitch = cos(halfPitch);
		float sinYaw = sin(halfYaw);
		float cosYaw = cos(halfYaw);

		x = sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw;
		y = cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw;
		z = cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw;
		w = cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw;

		Normalize();
	}

	Vector4<float> CommonUtilities::Quaternion::ToVector4() const
	{
		return Vector4<float>(x, y, z, w);
	}

	Vector3<float> CommonUtilities::Quaternion::ToEuler() const
	{
		float t0 = 2.f * (w * x + y * z);
		float t1 = 1.f - 2.f * (x * x + y * y);
		float rollX = atan2(t0, t1);

		float t2 = 2.f * (w * y - z * x);
		t2 = t2 > 1.f ? 1.f : t2;
		t2 = t2 < -1.f ? -1.f : t2;
		float pitchY = atan(t2);

		float t3 = 2.f * (w * z + x * y);
		float t4 = 1.f - 2.f * (y * y + z * z);
		float yawZ = atan2(t3, t4);

		return Vector3<float>(rollX, pitchY, yawZ);
	}

	/*CommonUtilities::Matrix4x4<float> CommonUtilities::Quaternion::ToMatrix() const
	{
		return CommonUtilities::Matrix4x4<float>::CreateRotationAsQuerternion(this->ToVector4());
	}*/

	Quaternion Quaternion::Lerp(Quaternion aQuat0, Quaternion aQuat1, float t)
	{
		aQuat0.Normalize();
		aQuat1.Normalize();

		// negate second quat if dot product is negative
		const float dot = aQuat0.ToVector4().Dot(aQuat1.ToVector4());

		if (dot < 0.0f)
		{
			aQuat1.Negate();
		}
		Quaternion finalResult;
		// c = aQuat0 + t(aQuat1 - aQuat0)  -->   c = aQuat0 - t(aQuat0 - aQuat1)
		// the latter is slightly better on x64

		finalResult.x = aQuat0.x - t * (aQuat0.x - aQuat1.x);
		finalResult.y = aQuat0.y - t * (aQuat0.y - aQuat1.y);
		finalResult.z = aQuat0.z - t * (aQuat0.z - aQuat1.z);
		finalResult.w = aQuat0.w - t * (aQuat0.w - aQuat1.w);

		finalResult.Normalize();

		return finalResult;
	}

	Quaternion Quaternion::Slerp(Quaternion aQuat0, Quaternion aQuat1, float t)
	{
		aQuat0.Normalize();
		aQuat1.Normalize();

		float dot = aQuat0.ToVector4().Dot(aQuat1.ToVector4());

		if (dot < 0.0f)
		{
			aQuat1.Negate();
			dot = -dot;
		}

		if (dot > 0.9995f)
		{
			return Lerp(aQuat0, aQuat1, t);
		}

		float theta = acosf(dot);
		float sinTheta = sinf(theta);
		float scale0 = sinf((1.0f - t) * theta) / sinTheta;
		float scale1 = sinf(t * theta) / sinTheta;

		return Quaternion(
			scale0 * aQuat0.x + scale1 * aQuat1.x,
			scale0 * aQuat0.y + scale1 * aQuat1.y,
			scale0 * aQuat0.z + scale1 * aQuat1.z,
			scale0 * aQuat0.w + scale1 * aQuat1.w
		);
	}
}