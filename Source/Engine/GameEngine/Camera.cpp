#include "Camera.h"
#include <iostream>

void Camera::SetOrtographicProjection(const CommonUtilities::Vector2<float> aResolution)
{
    myProjectionMatrix(1, 1) = 2.0f / aResolution.x;
    myProjectionMatrix(2, 2) = 2.0f / aResolution.y;
    myProjectionMatrix(3, 3) = -2.f / (myFarPlane - myNearPlane);
    myProjectionMatrix(4, 3) = myNearPlane / (myNearPlane - myFarPlane);

	myProjectionMatrix(3, 4) = 0;
	myProjectionMatrix(4, 4) = 1;

	myIsDirty = true;
}

void Camera::SetOrtographicProjection(float left, float right, float bottom, float top)
{
	myProjectionMatrix(1, 1) = 2.0f / (right - left);
	myProjectionMatrix(2, 2) = 2.0f / (top - bottom);
	myProjectionMatrix(3, 3) = 1.0f / (myFarPlane - myNearPlane);
	myProjectionMatrix(4, 4) = 1.0f;
	myProjectionMatrix(4, 1) = -(right + left) / (right - left);
	myProjectionMatrix(4, 2) = -(top + bottom) / (top - bottom);
	myProjectionMatrix(4, 3) = -myNearPlane / (myFarPlane - myNearPlane);
}

void Camera::SetPerspectiveProjection(const CommonUtilities::Vector2<float> aResolution)
{
	float zoom = static_cast<float>(1.f / tanf(myFOV / 2.f));
	float aspectRatioScalar = aResolution.x / aResolution.y;
	myProjectionMatrix(1, 1) = zoom;
	myProjectionMatrix(2, 2) = zoom * aspectRatioScalar;

	float farClipCamera = myFarPlane / (myFarPlane - myNearPlane);
	float nearClipCamera = (-myNearPlane * myFarPlane) / (myFarPlane - myNearPlane);

	myProjectionMatrix(3, 3) = farClipCamera;
	myProjectionMatrix(4, 3) = nearClipCamera;
	myProjectionMatrix(3, 4) = 1;
	myProjectionMatrix(4, 4) = 0;

	myIsDirty = true;
}

const CommonUtilities::Matrix4x4<float>& Camera::GetProjectionMatrix() const
{
	return myProjectionMatrix;
}

float Camera::GetFOV() const
{
	return myFOV;
}

float Camera::GetFarPlane() const
{
	return myFarPlane;
}

float Camera::GetNearPlane() const
{
	return myNearPlane;
}

bool Camera::GetIsDirty()
{
	return myIsDirty;
}

void Camera::RemoveIsDirty()
{
	myIsDirty = false;
}

Camera::Camera(bool aPerspective, const CommonUtilities::Vector2<float> /*aResolution*/) : myIsPerspective(aPerspective), myFOV(CommonUtilities::DegToRad(90.f)), myNearPlane(1.f), myFarPlane(1000000.f)
{
	
}