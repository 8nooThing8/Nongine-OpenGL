#pragma once

#include <initializer_list>
#include <cmath>
#include <iostream>

#include "Vector3.hpp"

namespace CommonUtilities
{
	template <class T>
	class Matrix4x4;

	template <class T>
	class Vector3;

	template <class T>
	class Vector2;

	class Quaternion;

	template <class T>
	class Vector4
	{
	public:
		T x, y, z, w;

		// Creates a null-vector
		Vector4()
		{
			x = 0;
			y = 0;
			z = 0;
			w = 0;
		}

		// Creates a vector (aX, aY, aZ, aW)
		Vector4(T aX, T aY, T aZ, T aW)
		{
			x = aX;
			y = aY;
			z = aZ;
			w = aW;
		}

		// Copy constructor Inisilizerlist
		Vector4(const std::initializer_list<T> &initList)
		{
			x = *(initList.begin());
			y = *(initList.begin() + 1);
			z = *(initList.begin() + 2);
			w = *(initList.begin() + 3);
		}

		// Creates a vector (aX, aY, aZ, aW)
		Vector4(const Vector3<T> &aVector, T aW = static_cast<T>(1))
		{
			x = aVector.x;
			y = aVector.y;
			z = aVector.z;
			w = aW;
		}

		void Print()
		{
			std::cout << "X: " << x << ",  Y: " << y << ",  Z:" << z << ",  W:" << w << "\n";
		}

		// Copy constructor (compiler generated)
		Vector4(const Vector4<T> &aVector) = default;

		// Assignment operator (compiler generated)
		Vector4<T> &operator=(const Vector4<T> &aVector4) = default;

		// Destructor (compiler generated)
		~Vector4() = default;

		template <class OtherVectorClass>
		explicit operator OtherVectorClass() const
		{
			OtherVectorClass converted;

			converted.x = x;
			converted.y = y;
			converted.z = z;
			converted.w = w;

			return converted;
		}

		// Returns a negated copy of the vector
		Vector4<T> operator-() const
		{
			return {-x, -y, -z, -w};
		}

		T &operator[](int index)
		{
			return *(&x + index);
		}

		const T operator[](int index) const
		{
			return *(&x + index);
		}

		inline Vector3<T> ToEuler() const
		{
			Vector3<T> angles;

			double sinr_cosp = 2 * ((*this).w * (*this).x + (*this).y * (*this).z);
			double cosr_cosp = 1 - 2 * ((*this).x * (*this).x + (*this).y * (*this).y);
			angles.z = static_cast<T>(std::atan2(sinr_cosp, cosr_cosp));

			double sinp = 2 * ((*this).w * (*this).y - (*this).z * (*this).x);
			if (std::abs(sinp) >= 1)
				angles.x = static_cast<T>(std::copysign(3.1415926f / 2, sinp));
			else
				angles.x = static_cast<T>(std::asin(sinp));

			double siny_cosp = 2 * ((*this).w * (*this).z + (*this).x * (*this).y);
			double cosy_cosp = 1 - 2 * ((*this).y * (*this).y + (*this).z * (*this).z);
			angles.y = static_cast<T>(std::atan2(siny_cosp, cosy_cosp));

			return angles;
		}

		// Returns the squared length of the vector
		T LengthSqrNoW() const
		{
			return (x * x) + (y * y) + (z * z);
		}

		// Returns the squared length of the vector
		T LengthSqr() const
		{
			return (x * x) + (y * y) + (z * z) + (w * w);
		}

		// Returns the length of the vector
		T Length() const
		{
			return static_cast<T>(std::sqrt((x * x) + (y * y) + (z * z) + (w * w)));
		}

		// Returns a normalized copy of this vector
		Vector4<T> GetNormalized() const
		{
			T magnitude = static_cast<T>(std::sqrt((x * x) + (y * y) + (z * z) + (w * w)));

			return {x / (magnitude), y / (magnitude), z / (magnitude), w / (magnitude)};
		}

		// Normalizes the vector
		void Normalize()
		{
			T magnitude = static_cast<T>(std::sqrt((x * x) + (y * y) + (z * z) + (w * w)));

			if (magnitude == 0)
				return;

			x = x / magnitude;
			y = y / magnitude;
			z = z / magnitude;
			w = w / magnitude;
		}

		// Zero the vector
		void Zero()
		{
			x = 0;
			y = 0;
			z = 0;
			w = 0;
		}

		const static CommonUtilities::Vector4<T> NormalOfVectors(const Vector4<T> &aVector0, const Vector4<T> &aVector1, const Vector4<T> &aVector2)
		{
			const CommonUtilities::Vector4<T> newVector0(aVector1 - aVector0);
			const CommonUtilities::Vector4<T> newVector1(aVector2 - aVector0);

			const CommonUtilities::Vector3<T> V30(newVector0.x, newVector0.y, newVector0.z);
			const CommonUtilities::Vector3<T> V31(newVector1.x, newVector1.y, newVector1.z);

			const CommonUtilities::Vector3<T> crossVec(V30.Cross(V31));

			const CommonUtilities::Vector4<T> crossVector(crossVec.x, crossVec.y, crossVec.z, 0);

			return crossVector.GetNormalized();
		}

		// Returns the dot product of this and aVector
		T Dot(const Vector4<T> &aVector) const
		{
			return ((x * aVector.x) + (y * aVector.y) + (z * aVector.z) + (w * aVector.w));
		}

		Vector4<T> GetSafeNormal() const
		{
			T length = this->Length();
			if (length > static_cast<T>(0))
			{
				return {x / length, y / length, z / length, w / length};
			}
			return *this; // Return the original vector if the length is zero
		}

		// Function to reflect a ray on a given axis
		static Vector4<T> reflect(const Vector4<T> &ray_direction, const Vector4<T> &axis)
		{
			Vector4<T> reflected_direction;

			// Calculate dot product of ray direction and axis
			T dot_product = ray_direction.Dot(axis);

			// Calculate the reflection
			reflected_direction.x = ray_direction.x - 2 * dot_product * axis.x;
			reflected_direction.y = ray_direction.y - 2 * dot_product * axis.y;
			reflected_direction.z = ray_direction.z - 2 * dot_product * axis.z;
			reflected_direction.w = ray_direction.w - 2 * dot_product * axis.w;

			return reflected_direction;
		}

		void operator=(Matrix4x4<T> &aMatrix)
		{
			x = aMatrix(4, 1);
			y = aMatrix(4, 2);
			z = aMatrix(4, 3);
			w = aMatrix(4, 4);
		}

		Vector2<T> ToVector2() const;
		Vector3<T> ToVector3() const;
		Quaternion ToQuat() const;

	private:
	};

	// Returns the Distance between two vectors
	template <class T>
	T Distance(const Vector4<T> &aVector0, const Vector4<T> &aVector1)
	{
		return std::sqrt(std::pow(aVector0.x - aVector1.x, 2) + std::pow((aVector0.y - aVector1.y), 2) + std::pow((aVector0.z - aVector1.z), 2) + std::pow((aVector0.w - aVector1.w), 2));
	}

	// Returns the vector sum of aVector0 and aVector1
	template <class T>
	Vector4<T> operator+(const Vector4<T> &aVector0, const Vector4<T> &aVector1)
	{
		return {aVector0.x + aVector1.x, aVector0.y + aVector1.y, aVector0.z + aVector1.z, aVector0.w + aVector1.w};
	}

	// Returns the vector difference of aVector0 and aVector1
	template <class T>
	Vector4<T> operator-(const Vector4<T> &aVector0, const Vector4<T> &aVector1)
	{
		return {aVector0.x - aVector1.x, aVector0.y - aVector1.y, aVector0.z - aVector1.z, aVector0.w - aVector1.w};
	}

	// Returns the vector aVector1 component-multiplied by aVector1
	template <class T>
	Vector4<T> operator*(const Vector4<T> &aVector0, const Vector4<T> &aVector1)
	{
		return {aVector0.x * aVector1.x, aVector0.y * aVector1.y, aVector0.z * aVector1.z, aVector0.w * aVector1.w};
	}

	// Returns the vector aVector multiplied by the scalar aScalar
	template <class T>
	Vector4<T> operator*(const Vector4<T> &aVector, const T &aScalar)
	{
		return {aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar, aVector.w * aScalar};
	}

	// Returns the vector aVector multiplied by the scalar aScalar
	template <class T>
	Vector4<T> operator*(const T &aScalar, const Vector4<T> &aVector)
	{
		return {aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar, aVector.w * aScalar};
	}

	// Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1 / aScalar)
	template <class T>
	Vector4<T> operator/(const Vector4<T> &aVector, const T &aScalar)
	{
		return {aVector.x / aScalar, aVector.y / aScalar, aVector.z / aScalar, aVector.w / aScalar};
	}

	template <class T>
	Vector4<T> operator/(const Vector4<T> &aVector0, const Vector4<T> &aVector1)
	{
		return {aVector0.x / aVector1.x, aVector0.y / aVector1.y, aVector0.z / aVector1.z, aVector0.w / aVector1.w};
	}

	template <class T>
	bool operator!=(const Vector4<T> &aVector0, const Vector4<T> &aVector1)
	{
		return !(aVector0 == aVector1);
	}

	template <class T>
	bool operator==(const Vector4<T> &aVector0, const Vector4<T> &aVector1)
	{
		if (aVector0.x != aVector1.x)
			return false;
		if (aVector0.y != aVector1.y)
			return false;
		if (aVector0.z != aVector1.z)
			return false;
		if (aVector0.w != aVector1.w)
			return false;

		return true;
	}

	template <class T>
	bool operator<(const Vector4<T> &aVector0, const Vector4<T> &aVector1)
	{
		if (aVector0.x > aVector1.x)
			return false;
		if (aVector0.y > aVector1.y)
			return false;
		if (aVector0.z > aVector1.z)
			return false;
		if (aVector0.w > aVector1.w)
			return false;

		return true;
	}

	template <class T>
	void operator+=(Vector4<T> &aVector0, const Vector4<T> &aVector1)
	{
		aVector0.x += aVector1.x;
		aVector0.y += aVector1.y;
		aVector0.z += aVector1.z;
		aVector0.w += aVector1.w;
	}

	template <class T>
	void operator+=(Vector4<T> &aVector0, const Vector3<T> aVector1)
	{
		aVector0.x += aVector1.x;
		aVector0.y += aVector1.y;
		aVector0.z += aVector1.z;
	}

	// Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <class T>
	void operator-=(Vector4<T> &aVector0, const Vector4<T> &aVector1)
	{
		aVector0.x -= aVector1.x;
		aVector0.y -= aVector1.y;
		aVector0.z -= aVector1.z;
		aVector0.w -= aVector1.w;
	}

	// Equivalent to setting aVector to (aVector * aScalar)
	template <class T>
	void operator*=(Vector4<T> &aVector, const T &aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
		aVector.z *= aScalar;
		aVector.w *= aScalar;
	}

	// Equivalent to setting aVector to (aVector / aScalar)
	template <class T>
	void operator/=(Vector4<T> &aVector, const T &aScalar)
	{
		aVector.x /= aScalar;
		aVector.y /= aScalar;
		aVector.z /= aScalar;
		aVector.w /= aScalar;
	}

	using Vector4d = Vector4<double>;
	using Vector4f = Vector4<float>;
	using Vector4i = Vector4<int>;
	using Vector4u = Vector4<unsigned>;
}
