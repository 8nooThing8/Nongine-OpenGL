#pragma once
#include <cassert>
#include <vector>
#include <ostream>

	template <class T>
	class Matrix4x4;
namespace CommonUtilities
{
	template <class T>
	class Vector4;

	template <class T>
	class Vector3;


	class Quaternion
	{
	public:
#pragma warning( push )
#pragma warning( disable : 4201)
		union
		{
			struct
			{
				float x;
				float y;
				float z;
				float w;
			};
			float data[4];
		};
#pragma warning( pop ) 

		Quaternion();
		constexpr Quaternion(const float& aX, const float& aY, const float& aZ, const float& aW);
		Quaternion(const std::vector<float>& aQuat);
		Quaternion(const Vector3<float>& aEuler);
		void InitWithEuler(const Vector3<float>& aEuler);

		Quaternion(const Quaternion& aQuat) = default;
		Quaternion& operator=(const Quaternion& aQuaternion) = default;
		~Quaternion() = default;

		//static Quaternion LookRotation(const Vector3<float>& aForward, const Vector3<float>& aUp);

		static Quaternion CreateFromAxisAngle(Vector3<float> anAxis, float anAngle);
		Quaternion GetInverse() const;
		void Negate();
		void Inverse();

		Vector4<float> ToVector4() const;
		Vector3<float> ToEuler() const;
		//CommonUtilities::Matrix4x4<float> ToMatrix() const;

		inline void Normalize()
		{
			float lenghtSqurt = x * x + y * y + z * z + w * w;

			float length = std::sqrt(lenghtSqurt);
			float inverseLength = 1.f / length;

			x *= inverseLength;
			y *= inverseLength;
			z *= inverseLength;
			w *= inverseLength;
		}

		static Quaternion Lerp(Quaternion aQuat0, Quaternion aQuat1, float t);
		static Quaternion Slerp(Quaternion aQuat0, Quaternion aQuat1, float t);

		template <class OtherVectorClass>
		explicit operator OtherVectorClass() const;

		float& operator[](const int anIndex);
		const float& operator[](const int anIndex) const;
	};

	template<class OtherVectorClass>
	inline Quaternion::operator OtherVectorClass() const
	{
		return OtherVectorClass(x, y, z, w);
	}

	inline float& Quaternion::operator[](const int anIndex)
	{
		assert(anIndex >= 0);
		assert(anIndex < 4);
		return data[anIndex];
	}


	inline const float& Quaternion::operator[](const int anIndex) const
	{
		assert(anIndex >= 0);
		assert(anIndex < 4);
		return data[anIndex];
	}

	Quaternion operator+(const Quaternion& aQuat0, const Quaternion& aQuat1) = delete;
	Quaternion operator-(const Quaternion& aQuat0, const Quaternion& aQuat1) = delete;
	Quaternion operator*(const Quaternion& aQuat, const float& aScalar) = delete;
	Quaternion operator*(const float& aScalar, const Quaternion& aQuat) = delete;
	Quaternion operator/(const Quaternion& aQuat, const float& aScalar) = delete;
	Quaternion operator/(const float& aScalar, const Quaternion& aQuat) = delete;

	inline Quaternion operator*(const Quaternion& aQuat0, const Quaternion& aQuat1)
	{
		float x1 = aQuat0.x;
		float y1 = aQuat0.y;
		float z1 = aQuat0.z;
		float w1 = aQuat0.w;

		float x2 = aQuat1.x;
		float y2 = aQuat1.y;
		float z2 = aQuat1.z;
		float w2 = aQuat1.w;

		float xR = w1 * x2 + x1 * w2 + y1 * z2 - z1 * y2;
		float yR = w1 * y2 - x1 * z2 + y1 * w2 + z1 * x2;
		float zR = w1 * z2 + x1 * y2 - y1 * x2 + z1 * w2;
		float wR = w1 * w2 - x1 * x2 - y1 * y2 - z1 * z2;

		Quaternion quat(xR, yR, zR, wR);

		quat.Normalize();

		return quat;
	}

	inline Quaternion operator/(const Quaternion& aQuat0, const Quaternion& aQuat1)
	{
		return aQuat0 * aQuat1.GetInverse();
	}

	void operator+=(Quaternion& aQuat0, const Quaternion& aQuat1) = delete;
	void operator-=(Quaternion& aQuat0, const Quaternion& aQuat1) = delete;
	void operator*=(Quaternion& aQuat, const float& aScalar) = delete;

	inline void operator*=(Quaternion& aQuat0, const Quaternion& aQuat1)
	{
		float x1 = aQuat0.x;
		float y1 = aQuat0.y;
		float z1 = aQuat0.z;
		float w1 = aQuat0.w;

		float x2 = aQuat1.x;
		float y2 = aQuat1.y;
		float z2 = aQuat1.z;
		float w2 = aQuat1.w;

		aQuat0.x = w1 * x2 + x1 * w2 + y1 * z2 - z1 * y2;
		aQuat0.y = w1 * y2 - x1 * z2 + y1 * w2 + z1 * x2;
		aQuat0.z = w1 * z2 + x1 * y2 - y1 * x2 + z1 * w2;
		aQuat0.w = w1 * w2 - x1 * x2 - y1 * y2 - z1 * z2;

		aQuat0.Normalize();
	}


	inline Quaternion::Quaternion() : x(float(0)), y(float(0)), z(float(0)), w(float(0)) {}

	constexpr inline Quaternion::Quaternion(const float& aX, const float& aY, const float& aZ, const float& aW) : x(aX), y(aY), z(aZ), w(aW) {}

	inline Quaternion::Quaternion(const std::vector<float>& aQuat) : x(aQuat[0]), y(aQuat[1]), z(aQuat[2]), w(aQuat[3]) {}

	inline Quaternion Quaternion::GetInverse() const
	{
		Quaternion inverseQuat;

		std::memcpy(inverseQuat.data, &data, sizeof(float) * 4);

		inverseQuat.x *= -1;
		inverseQuat.y *= -1;
		inverseQuat.z *= -1;

		inverseQuat.Normalize();

		return inverseQuat;
	}

	inline void Quaternion::Negate()
	{
		x *= -1;
		y *= -1;
		z *= -1;
		w *= -1;

		Normalize();
	}

	inline void Quaternion::Inverse()
	{
		x *= -1;
		y *= -1;
		z *= -1;

		Normalize();
	}


	inline std::ostream& operator<<(std::ostream& os, const Quaternion& aQuat)
	{
		return os << "{ X: " << aQuat.x << " Y: " << aQuat.y << " Z: " << aQuat.z << " W: " << aQuat.w << " }";
	}
}

namespace CU = CommonUtilities;