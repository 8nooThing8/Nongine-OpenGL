#include "GameCamera.h"

Ray<float> GameCamera::CameraPointToRay(const Vector2f& aScreenCoords)
{
	Vector2f ndc = aScreenCoords;

	ndc.y = (myScreenResolution.y - 1.f) - ndc.y;

	ndc.x = std::lerp(-1.f, 1.f, ndc.x / (myScreenResolution.x - 1.f));
	ndc.y = std::lerp(-1.f, 1.f, ndc.y / (myScreenResolution.y - 1.f));

	return CameraPointToRayInternal(ndc);
}

Ray<float> GameCamera::CameraPointToRayInternal(const Vector2f& aNdc)
{
	Vector4f point(aNdc.x, aNdc.y, -1.f, 1.f);  // NDC coordinates

	float farHeight = 2.0f * std::tan(myFOV * 0.5f * deg2rad) * myFarPlane;

	float aspectRation = 1920.f / 1080.f;

	float farWidth = farHeight * aspectRation;

	// Map NDC coordinates to the 3D coordinates on the far plane
	float x3D = point.x * (farWidth / 2.0f);  // Scale by half the width
	float y3D = point.y * (farHeight / 2.0f); // Scale by half the height
	float z3D = myFarPlane;                  // Far plane distance

	Vector3f point3D(x3D, y3D, z3D);
	Vector4f point4D = CU::Vector4f(point3D, 1.0f) * myProjectionMatrix;
	point3D = point4D.ToVector3() / point4D.w; // Perspective divide

	Vector3f dir = point3D - position.ToVector3();
	dir.Normalize();

	dir.Print();

	//dir.y = -dir.y;
	//dir.z = -dir.z;

	// Return the ray with origin at the camera position and the calculated direction
	return Ray<float>(position, dir);
}

void GameCamera::UpdateAlways()
{
	float cosX = cos(rotation.x * CommonUtilities::deg2rad);
	float cosY = cos(rotation.y * CommonUtilities::deg2rad);

	float sinX = sin(-rotation.x * CommonUtilities::deg2rad);
	float sinY = sin(rotation.y * CommonUtilities::deg2rad);

	myForward.x = cosX * sinY;
	myForward.y = -sinX;
	myForward.z = cosX * cosY;

	myRight.x = cosY;
	myRight.z = -sinY;

	myUp = myForward.Cross(myRight);

	float zoom = myProjectionMatrix(1, 1);

	float aspectRation = 1920.f / 1080.f;

	myProjectionMatrix(2, 2) = zoom * aspectRation;

	myInverse = CommonUtilities::Matrix4x4<float>();
	myInverse.SetPosition(-position);
	myInverse *= CU::Matrix4x4<float>::CreateRotation(CU::Vector4f(-rotation.x, rotation.y, rotation.z, 0) * deg2rad).GetInverse();
}

const CommonUtilities::Matrix4x4<float>& GameCamera::GetInverse()
{
	return myInverse;
}