#pragma once

#include <Matrix/Matrix4x4.hpp>
#include <Vector/Vector3.hpp>

class Camera
{
public:
	Camera(bool aPerspective = true, CommonUtilities::Vector2<float> aResolution = {1920, 1080});
	~Camera() = default;

	void SetOrtographicProjection(CommonUtilities::Vector2<float> aResolution);
	void SetOrtographicProjection(float left, float right, float bottom, float top);

	void SetPerspectiveProjection(const CommonUtilities::Vector2<float> aResolution);

	const CommonUtilities::Matrix4x4<float>& GetProjectionMatrix() const;

	float GetFOV() const;
	float GetFarPlane() const;
	float GetNearPlane() const;

	bool GetIsDirty();
	void RemoveIsDirty();


protected:
	CommonUtilities::Matrix4x4<float> myProjectionMatrix;

	CommonUtilities::Vector2f myScreenResolution;
	float myFOV;
	float myNearPlane;
	float myFarPlane;
	bool myIsPerspective;

	bool myIsDirty = true;

	CommonUtilities::Vector3<float> myForward;
	CommonUtilities::Vector3<float> myRight;
	CommonUtilities::Vector3<float> myUp;
};


