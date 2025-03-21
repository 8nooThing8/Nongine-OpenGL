#pragma once

#include <initializer_list>
#include <cmath>

#include "iostream"

namespace CommonUtilities
{
	template <class T>
	class Matrix3x3;

	template <class T>
	class Matrix4x4;


	template <class T>
	class Vector4;

	class Quaternion;

	template <class T>
	class Vector3

	{
	public:
		T x, y, z;

		//Creates a null-vector
		Vector3<T>()
		{
			x = 0;
			y = 0;
			z = 0;
		}

		//Creates a vector (aX, aY, aZ)
		Vector3<T>(T aX, T aY, T aZ)
		{
			x = aX;
			y = aY;
			z = aZ;
		}

		//Creates a vector (aX, aY, aZ)
		Vector3<T>(const Vector4<T>& aVector)
		{
			x = aVector.x;
			y = aVector.y;
			z = aVector.z;
		}

		//Copy constructor Inisilizerlist
		Vector3<T>(const std::initializer_list<T>& initList)
		{
			x = *(initList.begin());
			y = *(initList.begin() + 1);
			z = *(initList.begin() + 2);
		}

		//Copy constructor (compiler generated)
		Vector3<T>(const Vector3<T>& aVector) = default;

		void Print()
		{
			std::cout << "X: " << x << ",  Y: " << y << ",  Z:" << z << "\n";
		}

		bool operator==(const Vector3<T>& aVector3)
		{
			return x == aVector3.x && y == aVector3.y && z == aVector3.z;
		}

		//Assignment operator (compiler generated)
		Vector3<T>& operator=(const Vector3<T>& aVector3) = default;

		Vector3<T>& operator=(const Vector4<T>& aVector4)
		{
			x = aVector4.x;
			y = aVector4.y;
			z = aVector4.z;

			return *this;
		}

		~Vector3<T>() = default;

		template <class OtherVectorClass>
		Vector3<T>(const OtherVectorClass& aVector) : x(static_cast<T>(aVector.x)), y(static_cast<T>(aVector.y)), z(static_cast<T>(aVector.z))
		{
		}

		template <class OtherVectorClass>
		explicit operator OtherVectorClass() const
		{
			OtherVectorClass converted;

			converted.x = x;
			converted.y = y;
			converted.z = z;

			return converted;
		}

		Quaternion ToQuarternion() const;

		//Returns a negated copy of the vector
		Vector3<T> operator-() const
		{
			return { -x, -y, -z };
		}

		//Returns a negated copy of the vector
		T& operator[](int index)
		{
			switch (index)
			{
			case 0:
				return x;
			case 1:
				return y;
			case 2:
				return z;
			}

			return x;
		}

		inline Vector4<T> ToQuart()
		{
			// Extract Euler angles
			float roll = (*this).x; // Rotation around X-axis
			float pitch = (*this).y; // Rotation around Y-axis
			float yaw = (*this).z; // Rotation around Z-axis

			// Calculate half angles
			float halfRoll = roll * 0.5f;
			float halfPitch = pitch * 0.5f;
			float halfYaw = yaw * 0.5f;

			// Compute sine and cosine of half angles
			float sinRoll = sin(halfRoll);
			float cosRoll = cos(halfRoll);
			float sinPitch = sin(halfPitch);
			float cosPitch = cos(halfPitch);
			float sinYaw = sin(halfYaw);
			float cosYaw = cos(halfYaw);

			// Compute quaternion components
			CommonUtilities::Vector4<T> quaternion;
			quaternion.x = sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw;
			quaternion.y = cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw;
			quaternion.z = cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw;
			quaternion.w = cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw;

			quaternion.Normalize();

			return quaternion;
		}


		//Returns the squared length of the vector
		T LengthSqr() const
		{
			return (x * x) + (y * y) + (z * z);
		}

		//Returns the length of the vector
		T Length() const
		{
			return static_cast<T>(std::sqrt((x * x) + (y * y) + (z * z)));
		}

		//Returns a normalized copy of this vector
		Vector3<T> GetNormalized() const
		{
			T magnitude = std::sqrt((x * x) + (y * y) + (z * z));

			return { x / (magnitude), y / (magnitude), z / (magnitude) };
		}

		//Normalizes the vector
		void Normalize()
		{
			T magnitude = static_cast<T>(std::sqrt((x * x) + (y * y) + (z * z)));

			x = x / (magnitude);
			y = y / (magnitude);
			z = z / (magnitude);
		}

		//Zero the vector
		void Zero()
		{
			x = 0;
			y = 0;
			z = 0;
		}

		//Returns the dot product of this and aVector
		T Dot(const Vector3<T>& aVector) const
		{
			return ((x * aVector.x) + (y * aVector.y) + (z * aVector.z));
		}

		//Returns the cross product of this and aVector
		Vector3<T> Cross(const Vector3<T>& aVector) const
		{
			return { (y * aVector.z) - (z * aVector.y), (z * aVector.x) - (x * aVector.z), (x * aVector.y) - (y * aVector.x) };
		}

		////Returns the vector sum of aVector0 and aVector1
		//template <class T>
		//void operator=(const Vector3<T>& aVector)
		//{
		//	x = aVector.x;
		//	y = aVector.y;
		//	z = aVector.z;
		//}

		////Returns the vector sum of aVector0 and aVector1
		//template <class T>
		//void operator=(const Matrix4x4<T>& aMatrix)
		//{
		//	x = aMatrix(4, 1);
		//	y = aMatrix(4, 2);
		//	z = aMatrix(4, 3);
		//}

		//Vector4<T> Vector3<T>::ToVector4() const;
		//Vector4<T> Vector3<T>::ToVector4(float aW) const;

	private:

	};

	//Returns the Distance between two vectors
	template <class T>
	T Distance(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return static_cast<T>(std::sqrt(std::pow(aVector0.x - aVector1.x, 2) + std::pow((aVector0.y - aVector1.y), 2) + std::pow((aVector0.z - aVector1.z), 2)));
	}

	//Returns the vector sum of aVector0 and aVector1
	template <class T>
	Vector3<T> operator+(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return { aVector0.x + aVector1.x, aVector0.y + aVector1.y, aVector0.z + aVector1.z };
	}

	//Returns the vector difference of aVector0 and aVector1
	template <class T>
	Vector3<T> operator-(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return { aVector0.x - aVector1.x, aVector0.y - aVector1.y, aVector0.z - aVector1.z };
	}

	//Returns the vector aVector1 component-multiplied by aVector1
	template <class T>
	Vector3<T> operator*(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return { aVector0.x * aVector1.x, aVector0.y * aVector1.y, aVector0.z * aVector1.z };
	}

	template <class T>
	bool operator<(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return (aVector0.x < aVector1.x && aVector0.y < aVector1.y && aVector0.z < aVector1.z);
	}

	template <class T>
	bool operator>(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return (aVector0.x > aVector1.x && aVector0.y > aVector1.y && aVector0.z > aVector1.z);
	}

	template <class T>
	bool operator<=(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return (aVector0.x <= aVector1.x && aVector0.y <= aVector1.y && aVector0.z <= aVector1.z);
	}

	template <class T>
	bool operator>=(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return (aVector0.x >= aVector1.x && aVector0.y >= aVector1.y && aVector0.z >= aVector1.z);
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T>
	Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar)
	{
		return { aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar };
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T>
	Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector)
	{
		return { aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar };
	}

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1 / aScalar)
	template <class T>
	Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar)
	{
		return { aVector.x / aScalar, aVector.y / aScalar, aVector.z / aScalar };
	}

	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template <class T>
	void operator+=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		aVector0.x += aVector1.x;
		aVector0.y += aVector1.y;
		aVector0.z += aVector1.z;
	}

	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <class T>
	void operator-=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		aVector0.x -= aVector1.x;
		aVector0.y -= aVector1.y;
		aVector0.z -= aVector1.z;
	}

	//Equivalent to setting aVector to (aVector * aScalar)
	template <class T>
	void operator*=(Vector3<T>& aVector, const T& aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
		aVector.z *= aScalar;
	}

	//Equivalent to setting aVector to (aVector / aScalar)
	template <class T>
	void operator/=(Vector3<T>& aVector, const T& aScalar)
	{
		aVector.x /= aScalar;
		aVector.y /= aScalar;
		aVector.z /= aScalar;
	}

	using Vector3f = Vector3<float>;
}