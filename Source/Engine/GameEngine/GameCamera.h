#pragma once

#include "Camera.h"

#include "Matrix/Matrix4x4.hpp"
#include "Vector/Vector2.hpp"

namespace CommonUtilities
{
	template<class T>
	class Ray;

	class GameCamera : public Camera
	{
	public:
		GameCamera(bool aPerspective = true, CommonUtilities::Vector2<float> aResolution = { 1920, 1080 })
		{
			rotation = Vector4<float>();
			position = Vector4<float>(0, 0, 0, 1);

			if (aPerspective)
				SetPerspectiveProjection(aResolution);
			else
				SetOrtographicProjection(-aResolution.x * 0.5f, aResolution.x * 0.5f, -aResolution.y * 0.5f, aResolution.y * 0.5f);
		}
		~GameCamera() = default;

		void SetActiveCameraAsThis(bool aIsInspectorCamera = false) { main = this; myIsInspectorCamera = aIsInspectorCamera; };

		virtual void Update(float);

		virtual void Use();

		Ray<float> CameraPointToRay(const Vector2f& aScreenCoords);
		Ray<float> CameraPointToRayInternal(const Vector2f& aNdc);

		const Matrix4x4<float>& GetInverse() const;

		const CommonUtilities::Vector3<float>& GetForward() { return myForward; }
		const CommonUtilities::Vector3<float>& GetRight() { return myRight; }
		const CommonUtilities::Vector3<float>& GetUp() { return myUp; }

		static inline GameCamera* main = nullptr;

		Vector4<float> position;
		// Rotation in degrees
		Vector4<float> rotation;

		Matrix4x4<float> myInverse;

		bool myIsInspectorCamera = true;
	};
}
