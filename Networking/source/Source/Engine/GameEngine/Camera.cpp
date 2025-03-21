#include "../Engine.pch.h"

#include "Camera.h"
#include <iostream>

void Camera::SetOrtographicProjection(const CommonUtilities::Vector2<float> aResolution)
{
    myProjectionMatrix(1, 1) = 2.0f / aResolution.x;
    myProjectionMatrix(2, 2) = 2.0f / aResolution.y;
    myProjectionMatrix(3, 3) = 1.f / (myFarPlane - myNearPlane);
    myProjectionMatrix(4, 3) = myNearPlane / (myNearPlane - myFarPlane);

	myProjectionMatrix(3, 4) = 0;
	myProjectionMatrix(4, 4) = 1;
}

void Camera::SetPerspectiveProjection()
{
	float zoom = static_cast<float>(1.f / tanf(myFOV / 2.f));

	float aspectRatioScalar = 1920.f / 1080.f;

	myProjectionMatrix(1, 1) = zoom;

	myProjectionMatrix(2, 2) = zoom * aspectRatioScalar;

	float farClipCamera = myFarPlane / (myFarPlane - myNearPlane);
	float nearClipCamera = (-myNearPlane * myFarPlane) / (myFarPlane - myNearPlane);

	myProjectionMatrix(3, 3) = farClipCamera;
	myProjectionMatrix(4, 3) = nearClipCamera;

	myProjectionMatrix(3, 4) = 1;
	myProjectionMatrix(4, 4) = 0;
}

CommonUtilities::Matrix4x4<float>& Camera::GetProjectionMatrix()
{
	return myProjectionMatrix;
}

const float& Camera::GetFOV() const
{
	return myFOV;
}

const float& Camera::GetFarPlane() const
{
	return myFarPlane;
}

const float& Camera::GetNearPlane() const
{
	return myNearPlane;
}

Camera::Camera(const CommonUtilities::Vector2<float> aResolution, const float aNearClip) : myFOV(CommonUtilities::DegToRad(90.f)), myNearPlane(aNearClip), myFarPlane(1000000.f)
{
	myProjectionMatrix = CommonUtilities::Matrix4x4<float>();

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
}