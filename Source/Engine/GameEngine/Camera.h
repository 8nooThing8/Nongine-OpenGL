#pragma once

#include <Matrix/Matrix4x4.hpp>
#include <Vector/Vector3.hpp>

namespace CU = CommonUtilities;

class Camera
{
public:
	friend class DirectionalLight;
	friend class ReflectionProbe;

	Camera(CommonUtilities::Vector2<float> aResolution = {1920, 1080}, float aNearClip = 1.f);
	~Camera() = default;

	void SetOrtographicProjection(CommonUtilities::Vector2<float> aResolution);
	void SetPerspectiveProjection();

	CommonUtilities::Matrix4x4<float>& GetProjectionMatrix();

	const float& GetFOV() const;
	const float& GetFarPlane() const;
	const float& GetNearPlane() const;

	CommonUtilities::Vector3<float> myForward;
	CommonUtilities::Vector3<float> myRight;
	CommonUtilities::Vector3<float> myUp;

protected:
	CU::Vector2f myScreenResolution;

	float myFOV;

	float myNearPlane;
	float myFarPlane;

	CommonUtilities::Matrix4x4<float> myProjectionMatrix;
};


