#pragma once

#include <Vector/Vector4.hpp>
#include <array>
#include "UtilityFunctions.hpp"

#include <vector>

#include <assert.h>

namespace CommonUtilities
{
	template <class T>
	class Matrix4x4
	{
	public:
		~Matrix4x4() = default;

		// Creates the identity matrix.
		Matrix4x4<T>()
		{
			myMatrix = std::array<std::array<T, 4>, 4>{{
				{1.0f, 0.0f, 0.0f, 0.0f},
				{ 0.0f, 1.0f, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f, 1.0f }
				}};
		}

		// Copy Constructor.
		Matrix4x4<T>(const Matrix4x4<T>& aMatrix)
		{
			std::memcpy(&myData[0], &aMatrix.myData[0], sizeof(T) * 16);
		}

		// Constructor using array.
		Matrix4x4<T>(const std::array<std::array<T, 4>, 4>& aArray)
		{
			myMatrix = std::array<std::array<T, 4>, 4>(aArray);
		}

		operator float* () { return myData; }

		void SetRight(float aX, float aY, float aZ)
		{
			(*this)(1, 1) = aX;
			(*this)(2, 1) = aY;
			(*this)(3, 1) = aZ;
		}

		void SetUp(float aX, float aY, float aZ)
		{
			(*this)(1, 2) = aX;
			(*this)(2, 2) = aY;
			(*this)(3, 2) = aZ;
		}

		void SetForward(float aX, float aY, float aZ)
		{
			(*this)(1, 3) = aX;
			(*this)(2, 3) = aY;
			(*this)(3, 3) = aZ;
		}

		CommonUtilities::Vector3<T> ToEuler()
		{
			CommonUtilities::Vector3<T> eulerAngles;

			// Extract pitch (y-axis rotation)
			float pitch;
			if ((*this)(3, 1) < 1.0f)
			{
				if ((*this)(3, 1) > -1.0f)
				{
					pitch = asinf(-(*this)(3, 1));
					eulerAngles.y = pitch;

					// Calculate yaw and roll
					eulerAngles.x = atan2f((*this)(1, 2), (*this)(3, 3)); // roll (around x-axis)
					eulerAngles.z = atan2f((*this)(2, 1), (*this)(1, 1)); // yaw (around z-axis)
				}
				else
				{
					pitch = -1.5707963f;
					eulerAngles.y = pitch;

					eulerAngles.x = -atan2f(-(*this)(2, 3), (*this)(2, 2)); // Combine roll and yaw
					eulerAngles.z = 0.0f;
				}
			}
			else
			{
				pitch = 1.5707963f;
				eulerAngles.y = pitch;

				eulerAngles.x = atan2f(-(*this)(2, 3), (*this)(2, 2)); // Combine roll and yaw
				eulerAngles.z = 0.0f;
			}

			return eulerAngles;
		}

		// () operator for accessing element (row, column) for read/write or read, respectively.
		inline T& operator()(const int aRow, const int aColumn)
		{
			assert(aRow <= 4 && aColumn <= 4 && "Index out of range.");
			assert(aRow > 0 && aColumn > 0 && "Index out of range.");

			return myMatrix[aRow - 1][aColumn - 1];
		}

		inline const T& operator()(const int aRow, const int aColumn) const
		{
			assert(aRow <= 4 && aColumn <= 4 && "Index out of range.");
			assert(aRow > 0 && aColumn > 0 && "Index out of range.");

			return myMatrix[aRow - 1][aColumn - 1];
		}

		inline T& operator[](const unsigned& aIndex)
		{
			return myMatrix[static_cast<int>(aIndex / 4)][static_cast<int>(aIndex % 4)];
		}

		inline const T& operator[](const unsigned& aIndex) const
		{
			return myMatrix[static_cast<int>(aIndex / 4)][static_cast<int>(aIndex % 4)];
		}

		inline const float GetTrace() const
		{
			float trace = 0;

			trace += (*this)(1, 1);
			trace += (*this)(2, 2);
			trace += (*this)(3, 3);

			return trace;
		}

		inline Vector4<T> GetScale() const
		{
			Vector4<T> scale = Vector4<T>((*this)(1, 1), (*this)(2, 2), (*this)(3, 3), 1);

			return scale;
		}

		inline void SetScale(const Vector4<T>& aSize)
		{
			(*this)(1, 1) = aSize.x;
			(*this)(2, 2) = aSize.y;
			(*this)(3, 3) = aSize.z;
		}

		inline const Vector4<float> GetRotationAsQuerternion()
		{
			Vector4<float> quarternion = { 0, 0, 0, 0 };

			float trace = GetTrace();
			float scalar = 0;

			if (trace >= 0)
			{
				scalar = sqrtf(trace + 1.0f) * 2;

				quarternion.w = scalar * 0.25f;
				quarternion.x = ((*this)(3, 2) + (*this)(2, 3)) / scalar;
				quarternion.y = ((*this)(1, 3) + (*this)(3, 1)) / scalar;
				quarternion.z = ((*this)(2, 1) + (*this)(1, 2)) / scalar;
			}
			else
			{
				float maxValue = (*this)(1, 1);
				int maxIndex = 1;

				for (int i = 2; i <= 3; ++i)
				{
					if ((*this)(i, i) > maxValue)
					{
						maxValue = (*this)(i, i);
						maxIndex = i;
					}
				}

				switch (maxIndex)
				{
				case 1:
					scalar = sqrtf(1.0f + (*this)(1, 1) - (*this)(2, 2) - (*this)(3, 3)) * 2;

					quarternion.w = ((*this)(3, 2) - (*this)(2, 3)) / scalar;
					quarternion.x = scalar * 0.25f;
					quarternion.y = ((*this)(1, 2) + (*this)(2, 1)) / scalar;
					quarternion.z = ((*this)(1, 3) + (*this)(3, 1)) / scalar;
					break;
				case 2:
					scalar = sqrtf(1.0f + (*this)(2, 2) - (*this)(1, 1) - (*this)(3, 3)) * 2;

					quarternion.w = ((*this)(1, 3) - (*this)(3, 1)) / scalar;
					quarternion.x = ((*this)(1, 2) + (*this)(2, 1)) / scalar;
					quarternion.y = scalar * 0.25f;
					quarternion.z = ((*this)(2, 3) + (*this)(3, 2)) / scalar;
					break;
				case 3:
					scalar = sqrtf(1.0f + (*this)(3, 3) - (*this)(1, 1) - (*this)(2, 2)) * 2;

					quarternion.w = ((*this)(2, 1) - (*this)(1, 2)) / scalar;
					quarternion.x = ((*this)(1, 3) + (*this)(3, 1)) / scalar;
					quarternion.y = ((*this)(2, 3) + (*this)(3, 2)) / scalar;
					quarternion.z = scalar * 0.25f;
					break;
				}
			}

			return quarternion;
		}

		inline Matrix4x4<T> GetTranspose() const
		{
			Matrix4x4<T> newMatrix;

			std::memcpy(&newMatrix.myData[0], &(*this).myData[0], sizeof(T) * 16);

			// Swap elements to transpose the matrix
			std::swap(newMatrix.myMatrix[1][0], newMatrix.myMatrix[0][1]);
			std::swap(newMatrix.myMatrix[2][0], newMatrix.myMatrix[0][2]);
			std::swap(newMatrix.myMatrix[3][0], newMatrix.myMatrix[0][3]);
			std::swap(newMatrix.myMatrix[2][1], newMatrix.myMatrix[1][2]);
			std::swap(newMatrix.myMatrix[3][1], newMatrix.myMatrix[1][3]);
			std::swap(newMatrix.myMatrix[3][2], newMatrix.myMatrix[2][3]);

			return newMatrix;
		}

		void SetRotationAsQuerternion(const Vector4<float>& aQuarternion)
		{
			Matrix4x4<T> newMatrix;

			// Extract quaternion components
			float x = aQuarternion.x;
			float y = aQuarternion.y;
			float z = aQuarternion.z;
			float w = aQuarternion.w;

			// Compute 3x3 rotation matrix from quaternion
			float xx = x * x;
			float yy = y * y;
			float zz = z * z;
			float xy = x * y;
			float xz = x * z;
			float yz = y * z;
			float wx = w * x;
			float wy = w * y;
			float wz = w * z;

			float r11 = 1.0f - 2.0f * (yy + zz);
			float r12 = 2.0f * (xy - wz);
			float r13 = 2.0f * (xz + wy);

			float r21 = 2.0f * (xy + wz);
			float r22 = 1.0f - 2.0f * (xx + zz);
			float r23 = 2.0f * (yz - wx);

			float r31 = 2.0f * (xz - wy);
			float r32 = 2.0f * (yz + wx);
			float r33 = 1.0f - 2.0f * (xx + yy);

			(*this)(1, 1) = r11;
			(*this)(1, 2) = r12;
			(*this)(1, 3) = r13;
			(*this)(1, 4) = 0;

			(*this)(2, 1) = r21;
			(*this)(2, 2) = r22;
			(*this)(2, 3) = r23;
			(*this)(2, 4) = 0;

			(*this)(3, 1) = r31;
			(*this)(3, 2) = r32;
			(*this)(3, 3) = r33;
			(*this)(3, 4) = 0;

			(*this) = GetTranspose();
		}

		static Matrix4x4<T> CreateRotationAsQuerternion(const Vector4<float>& aQuarternion)
		{
			Matrix4x4<T> newMatrix;

			// Extract quaternion components
			float x = aQuarternion.x;
			float y = aQuarternion.y;
			float z = aQuarternion.z;
			float w = aQuarternion.w;

			// Compute 3x3 rotation matrix from quaternion
			float xx = x * x;
			float yy = y * y;
			float zz = z * z;
			float xy = x * y;
			float xz = x * z;
			float yz = y * z;
			float wx = w * x;
			float wy = w * y;
			float wz = w * z;

			float r11 = 1.0f - 2.0f * (yy + zz);
			float r12 = 2.0f * (xy - wz);
			float r13 = 2.0f * (xz + wy);

			float r21 = 2.0f * (xy + wz);
			float r22 = 1.0f - 2.0f * (xx + zz);
			float r23 = 2.0f * (yz - wx);

			float r31 = 2.0f * (xz - wy);
			float r32 = 2.0f * (yz + wx);
			float r33 = 1.0f - 2.0f * (xx + yy);

			newMatrix(1, 1) = r11;
			newMatrix(1, 2) = r12;
			newMatrix(1, 3) = r13;
			newMatrix(1, 4) = 0;

			newMatrix(2, 1) = r21;
			newMatrix(2, 2) = r22;
			newMatrix(2, 3) = r23;
			newMatrix(2, 4) = 0;

			newMatrix(3, 1) = r31;
			newMatrix(3, 2) = r32;
			newMatrix(3, 3) = r33;
			newMatrix(3, 4) = 0;

			newMatrix = newMatrix.GetTranspose();

			return newMatrix;
		}

		// Static functions for creating rotation matrices.
		inline static Matrix4x4<T> CreateRotationAroundX(T aAngleInRadians)
		{
			Matrix4x4<T> newMatrix;

			newMatrix(1, 1) = 1;
			newMatrix(2, 2) = std::cos(aAngleInRadians);
			newMatrix(2, 3) = std::sin(aAngleInRadians);
			newMatrix(3, 2) = -std::sin(aAngleInRadians);
			newMatrix(3, 3) = std::cos(aAngleInRadians);
			newMatrix(4, 4) = 1;

			return std::move(newMatrix);
		}

		inline static Matrix4x4<T> CreateRotationAroundY(T aAngleInRadians)
		{
			Matrix4x4<T> newMatrix;

			newMatrix(1, 1) = std::cos(aAngleInRadians);
			newMatrix(1, 3) = -std::sin(aAngleInRadians);
			newMatrix(2, 2) = 1;
			newMatrix(3, 1) = std::sin(aAngleInRadians);
			newMatrix(3, 3) = std::cos(aAngleInRadians);
			newMatrix(4, 4) = 1;

			return std::move(newMatrix);
		}

		inline static Matrix4x4<T> CreateRotationAroundZ(T aAngleInRadians)
		{
			Matrix4x4<T> newMatrix;

			newMatrix(1, 1) = std::cos(aAngleInRadians);
			newMatrix(1, 2) = std::sin(aAngleInRadians);
			newMatrix(2, 1) = -std::sin(aAngleInRadians);
			newMatrix(2, 2) = std::cos(aAngleInRadians);
			newMatrix(3, 3) = 1;
			newMatrix(4, 4) = 1;

			return std::move(newMatrix);
		}

		inline static Matrix4x4<T> CreateRotation(T aAngleInRadiansX, T aAngleInRadiansY, T aAngleInRadiansZ)
		{
			Matrix4x4<T> newMatrix;

			newMatrix = CreateRotationAroundX(aAngleInRadiansX) * CreateRotationAroundY(aAngleInRadiansY) * CreateRotationAroundZ(aAngleInRadiansZ);

			return std::move(newMatrix);
		}

		inline static Matrix4x4<T> CreateRotation(const Vector4<float>& aRotation)
		{
			Matrix4x4<T> newMatrix;

			newMatrix = CreateRotationAroundX(aRotation.x) * CreateRotationAroundY(aRotation.y) * CreateRotationAroundZ(aRotation.z);

			return std::move(newMatrix);
		}

		inline static Matrix4x4<T> CreateRotation(const Vector3<float>& aRotation)
		{
			Matrix4x4<T> newMatrix;

			newMatrix = CreateRotationAroundX(aRotation.x) * CreateRotationAroundY(aRotation.y) * CreateRotationAroundZ(aRotation.z);

			return std::move(newMatrix);
		}

		inline Matrix4x4<T> createRotationMatrix(T angle, const Vector4<float>& aCoords)
		{
			aCoords.Normalize();

			float x = aCoords.x;
			float y = aCoords.y;
			float z = aCoords.z;

			T length = std::sqrt(x * x + y * y + z * z);

			x /= length;
			y /= length;
			z /= length;

			// Compute trigonometric values
			T c = std::cos(angle);
			T s = std::sin(angle);
			T t = 1 - c;

			Matrix4x4<T> rotationMatrix;

			rotationMatrix(1, 1) = t * x * x + c;
			rotationMatrix(1, 2) = t * x * y - s * z;
			rotationMatrix(1, 3) = t * x * z + s * y;

			rotationMatrix(2, 1) = t * x * y + s * z;
			rotationMatrix(2, 2) = t * y * y + c;
			rotationMatrix(2, 3) = t * y * z - s * x;

			rotationMatrix(3, 1) = t * x * z - s * y;
			rotationMatrix(3, 2) = t * y * z + s * x;
			rotationMatrix(3, 3) = t * z * z + c;

			return rotationMatrix;
		}

		inline Matrix4x4<T> GetInverse()
		{
			Matrix4x4 inversed = GetTranspose();
			Matrix4x4 translation;

			translation(4, 1) = -inversed(1, 4);
			translation(4, 2) = -inversed(2, 4);
			translation(4, 3) = -inversed(3, 4);

			inversed(1, 4) = 0;
			inversed(2, 4) = 0;
			inversed(3, 4) = 0;

			return inversed * translation;
		}

		inline const Vector4<T> GetPosition() const
		{
			Vector4<T> pos;

			pos.x = (*this)(4, 1);
			pos.y = (*this)(4, 2);
			pos.z = (*this)(4, 3);
			pos.w = (*this)(4, 4);

			return pos;
		}

		inline void SetPosition(const Vector4<T>& pos)
		{
			(*this)(4, 1) = pos.x;
			(*this)(4, 2) = pos.y;
			(*this)(4, 3) = pos.z;
			(*this)(4, 4) = 1;
		}

		inline void SetPosition(const Vector3<T>& pos)
		{
			(*this)(4, 1) = pos.x;
			(*this)(4, 2) = pos.y;
			(*this)(4, 3) = pos.z;
			(*this)(4, 4) = 1;
		}

		inline const Vector4<T> GetRight()
		{
			Vector4<T> direction;

			direction.x = (*this)(1, 1);
			direction.y = (*this)(2, 1);
			direction.z = (*this)(3, 1);
			direction.w = 0;

			return direction;
		}

		inline const Vector4<T> GetUp()
		{
			Vector4<T> direction;

			direction.x = (*this)(1, 2);
			direction.y = (*this)(2, 2);
			direction.z = (*this)(3, 2);
			direction.w = 0;

			return direction;
		}

		inline const Vector4<T> GetForward()
		{
			Vector4<T> direction;

			direction.x = (*this)(1, 3);
			direction.y = (*this)(2, 3);
			direction.z = (*this)(3, 3);
			direction.w = 0;

			return direction;
		}

		inline const std::array<std::array<T, 4>, 4>* GetMatrix() const
		{
			return &myMatrix;
		}

		inline void operator =(const Matrix4x4<T>& aMatrix)
		{
			myMatrix = std::array<std::array<T, 4>, 4>(*aMatrix.GetMatrix());
		}

		inline void operator =(const Vector3<T>& aVector)
		{
			for (int x = 0; x < 3; x++)
			{
				myMatrix[4][x] = aVector[x];
			}
		}

		inline void operator =(const Vector4<T>& aVector)
		{
			for (int x = 0; x < 4; x++)
			{
				myMatrix[4][x] = aVector[x];
			}
		}

		inline void operator +=(const Matrix4x4<T>& aMatrix)
		{
			for (int y = 0; y < 4; y++)
			{
				for (int x = 0; x < 4; x++)
				{
					myMatrix[y][x] += aMatrix(y + 1, x + 1);
				}
			}
		}

		inline void operator -=(const Matrix4x4<T>& aMatrix)
		{
			for (int y = 0; y < 4; y++)
			{
				for (int x = 0; x < 4; x++)
				{
					myMatrix[y][x] -= aMatrix(y + 1, x + 1);
				}
			}
		}

		inline void operator *=(const Matrix4x4<T>& aMatrix)
		{
			Matrix4x4<T> newMatrix;
			T value = 0;

			for (int row = 1; row <= 4; row++)
			{
				for (int column = 1; column <= 4; column++)
				{
					value = 0;

					for (int i = 1; i <= 4; i++)
					{
						value += (*this)(row, i) * aMatrix(i, column);
					}

					newMatrix(row, column) = value;
				}
			}

			myMatrix = *newMatrix.GetMatrix();
		}

		inline void operator *=(T aScalar)
		{
			for (int y = 0; y < 4; y++)
			{
				for (int x = 0; x < 4; x++)
				{
					if (x == y)
					{
						myMatrix[y][x] *= aScalar;
					}
				}
			}
		}

		inline void operator *=(const Vector4<T>& aVector4)
		{
			CommonUtilities::Vector4<T> newVector;
			T value = 0;

			for (int column = 1; column < 5; column++)
			{
				value = 0;

				for (int row = 1; row < 5; row++)
				{
					value += myMatrix(row, column) * aVector4[row];
				}

				newVector[column] = value;
			}

			return newVector;
		}

		union
		{
			float myData[16];

			std::array<std::array<T, 4>, 4> myMatrix;

			CommonUtilities::Vector4<float> rows[4];
		};
	private:

	};

	template <class T>
	inline const Matrix4x4<T> operator +(const Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		Matrix4x4<T> addedMatrixes;

		for (int y = 0; y < 4; y++)
		{
			for (int x = 0; x < 4; x++)
			{
				addedMatrixes(y + 1, x + 1) = aMatrix0(y + 1, x + 1) + aMatrix1(y + 1, x + 1);
			}
		}

		return addedMatrixes;
	}

	template <class T>
	const Matrix4x4<T> operator -(const Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		Matrix4x4<T> subtractedMatrixes;

		for (int y = 0; y < 4; y++)
		{
			for (int x = 0; x < 4; x++)
			{
				subtractedMatrixes(y + 1, x + 1) = aMatrix0(y + 1, x + 1) - aMatrix1(y + 1, x + 1);
			}
		}

		return subtractedMatrixes;
	}

	template <typename T>
	inline Matrix4x4<T> operator*(const Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		Matrix4x4<T> newMatrix;

		Matrix4x4<T> testMat = aMatrix1.GetTranspose();

		for (int row = 0; row < 4; row++)
		{
			for (int column = 0; column < 4; column++)
			{
				newMatrix.myMatrix[row][column] =
					aMatrix0.myMatrix[row][0] * testMat.myMatrix[column][0] +
					aMatrix0.myMatrix[row][1] * testMat.myMatrix[column][1] +
					aMatrix0.myMatrix[row][2] * testMat.myMatrix[column][2] +
					aMatrix0.myMatrix[row][3] * testMat.myMatrix[column][3];
			}
		}

		return newMatrix;
	}


	template <class T>
	const inline Vector4<T> operator *(const Vector4<T>& aVector4, const Matrix4x4<T>& aMatrix0)
	{
		CommonUtilities::Vector4<T> newVector;

		T value = 0;

		for (int column = 1; column < 5; column++)
		{
			value = 0;

			for (int row = 1; row < 5; row++)
			{
				// Add one since matrices index starts at 1.
				value += aMatrix0(row, column) * aVector4[row - 1];
			}

			newVector[column - 1] = value;
		}

		return newVector;
	}

	template <class T>
	const inline bool operator ==(const Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		for (int y = 0; y < 4; y++)
		{
			for (int x = 0; x < 4; x++)
			{
				if (aMatrix0(y + 1, x + 1) != aMatrix1(y + 1, x + 1))
				{
					return false;
				}
			}
		}

		return true;
	}
}