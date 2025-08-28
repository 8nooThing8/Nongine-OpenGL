#pragma once

#include <Vector/Vector3.hpp>

namespace CommonUtilities
{
	class Shape
	{
	public:
		Shape() = default;
		~Shape() = default;

		void SetPosition(const Vector3<float>& aPosition)
		{
			myPosition = aPosition;
		}

		const Vector3<float>& GetPosition() const
		{
			return myPosition;
		}

	protected:
		Vector3<float> myPosition;
	};
}
