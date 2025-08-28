#pragma once

#include <initializer_list>

#include <cmath>
#include <cstring>

namespace CommonUtilities
{
	template <class T>

	class Vector2

	{
	public:
		T x;
		T y;

		// Creates a null-vector
		Vector2()
		{
			x = 0;
			y = 0;
		}

		// Creates a vector (aX, aY)
		Vector2(const T &aX, const T &aY)
		{
			x = aX;
			y = aY;
		}

		// Copy constructor Inisilizerlist
		Vector2(const std::initializer_list<T> initList)
		{
			std::memcpy(&x, initList.begin(), sizeof(T) * 2);
		}

		// Copy constructor (compiler generated)
		Vector2(const Vector2<T> &aVector) = default;

		bool operator==(const Vector2<T> &aVector2)
		{
			return x == aVector2.x && y == aVector2.y;
		}

		// Assignment operator (compiler generated)
		Vector2<T> &operator=(const Vector2<T> &aVector2) = default;

		// Destructor (compiler generated)
		~Vector2() = default;

		// Explicit Type operator, create a different vector with the same values.
		// Example creates a Tga::Vector2<T> from this CommonUtillities::Vector2<T>
		template <class OtherVectorClass>
		explicit operator OtherVectorClass() const
		{
			OtherVectorClass converted;

			converted.x = x;
			converted.y = y;

			return converted;
		}

		// Returns a negated copy of the vector
		Vector2<T> operator-() const
		{
			return {-x, -y};
		}

		// Returns a negated copy of the vector
		T &operator[](int index)
		{
			switch (index)
			{
			case 0:
				return static_cast<T &>(x);
			case 1:
				return static_cast<T &>(y);
			}

			return static_cast<T &>(x);
		}

		// Returns the squared length of the vector
		T LengthSqr() const
		{
			return (x * x) + (y * y);
		}

		// Returns the length of the vector
		T Length() const
		{
			return static_cast<T>(std::sqrt((x * x) + (y * y)));
		}

		// Returns a normalized copy of this vector
		Vector2<T> GetNormalized() const
		{
			T magnitude = static_cast<T>(std::sqrt((x * x) + (y * y)));

			return {x / (magnitude), y / (magnitude)};
		}

		// Normalizes the vector
		void Normalize()
		{
			T magnitude = static_cast<T>(std::sqrt((x * x) + (y * y)));

			if (magnitude == 0)
				return;

			x = x / magnitude;
			y = y / magnitude;
		}

		// Zero the vector
		void Zero()
		{
			x = 0;
			y = 0;
		}

		// Returns the dot product of this and aVector
		T Dot(const Vector2<T> &aVector) const
		{
			return ((x * aVector.x) + (y * aVector.y));
		}

	private:
	};

	template <class T>
	T DistanceSqrd(const Vector2<T> &aVector0, const Vector2<T> &aVector1)
	{
		const T xDistance = aVector0.x - aVector1.x;
		const T yDistance = aVector0.y - aVector1.y;

		return xDistance * xDistance + yDistance * yDistance;
	}

	// Returns the Distance between two vectors
	template <class T>
	T Distance(const Vector2<T> &aVector0, const Vector2<T> &aVector1)
	{
		const T xDistance = aVector0.x - aVector1.x;
		const T yDistance = aVector0.y - aVector1.y;

		return std::sqrt(xDistance * xDistance + yDistance * yDistance);
	}

	// Returns the vector sum of aVector0 and aVector1
	template <class T>
	Vector2<T> operator+(const Vector2<T> &aVector0, const Vector2<T> &aVector1)
	{
		return {aVector0.x + aVector1.x, aVector0.y + aVector1.y};
	}

	// Returns the vector difference of aVector0 and aVector1
	template <class T>
	Vector2<T> operator-(const Vector2<T> &aVector0, const Vector2<T> &aVector1)
	{
		return {aVector0.x - aVector1.x, aVector0.y - aVector1.y};
	}

	// Returns the vector aVector1 component-multiplied by aVector1
	template <class T>
	Vector2<T> operator*(const Vector2<T> &aVector0, const Vector2<T> &aVector1)
	{
		return {aVector0.x * aVector1.x, aVector0.y * aVector1.y};
	}

	// Returns the vector aVector multiplied by the scalar aScalar
	template <class T>
	Vector2<T> operator*(const Vector2<T> &aVector, const T &aScalar)
	{
		return {aVector.x * aScalar, aVector.y * aScalar};
	}

	// Returns the vector aVector multiplied by the scalar aScalar
	template <class T>
	Vector2<T> operator*(const T &aScalar, const Vector2<T> &aVector)
	{
		return {aVector.x * aScalar, aVector.y * aScalar};
	}

	// Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1 / aScalar)
	template <class T>
	Vector2<T> operator/(const Vector2<T> &aVector, const T &aScalar)
	{
		return {aVector.x / aScalar, aVector.y / aScalar};
	}

	// Equivalent to setting aVector0 to (aVector0 + aVector1)
	template <class T>
	void operator+=(Vector2<T> &aVector0, const Vector2<T> &aVector1)
	{
		aVector0.x += aVector1.x;
		aVector0.y += aVector1.y;
	}

	// Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <class T>
	void operator-=(Vector2<T> &aVector0, const Vector2<T> &aVector1)
	{
		aVector0.x -= aVector1.x;
		aVector0.y -= aVector1.y;
	}

	// Equivalent to setting aVector to (aVector * aScalar)
	template <class T>
	void operator*=(Vector2<T> &aVector, const T &aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
	}

	// Equivalent to setting aVector to (aVector / aScalar)
	template <class T>
	void operator/=(Vector2<T> &aVector, const T &aScalar)
	{
		aVector.x /= aScalar;
		aVector.y /= aScalar;
	}

	using Vector2d = Vector2<double>;
	using Vector2f = Vector2<float>;
	using Vector2i = Vector2<int>;
	using Vector2u = Vector2<unsigned>;
}
