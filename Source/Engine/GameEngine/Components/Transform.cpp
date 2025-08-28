#include "Transform.h"

#include "../Engine/GameEngine/GameCamera.h"

#include "GameObject.h"

using namespace CommonUtilities;

void Transform::Start()
{

}

void Transform::Update(const float /*aDeltaTime*/)
{
	if (myIsDirty)
	{
		myObjectMatrix = CU::Matrix4x4<float>();

		myObjectMatrix(1, 1) = myScale.x;
		myObjectMatrix(2, 2) = myScale.y;
		myObjectMatrix(3, 3) = myScale.z;

		myObjectMatrix *= CommonUtilities::Matrix4x4<float>::CreateRotationAroundZ(myRotation.z);
		myObjectMatrix.SetPosition(myPosition);

		gameObject->RunOnTransform();

		myIsDirty = false;
	}
}

const CommonUtilities::Matrix4x4<float>& Transform::GetMatrix()
{
	return myObjectMatrix;
}

const CommonUtilities::Matrix4x4<float>& Transform::GetMatrixNoScale()
{
	return myObjectNoScaleMatrix;
}

void Transform::SetPosition(const Vector3<float>& aPosition)
{
	myPosition = aPosition;
	SetIsDirty();
}

void Transform::SetRotation(const Vector3<float>& aRotation)
{
	myRotation = CU::Vector4f(aRotation, 0.f);
	SetIsDirty();
}

void Transform::SetScale(const Vector3<float>& aScale)
{
	myScale = aScale;
	SetIsDirty();
}

void Transform::SetPosition(const Vector4<float>& aPosition)
{
	myPosition = aPosition;
	SetIsDirty();
}

void Transform::SetRotation(const Vector4<float>& aRotation)
{
	myRotation = CU::Vector4f(aRotation, 0.f);
	SetIsDirty();
}

void Transform::SetScale(const Vector4<float>& aScale)
{
	myScale = aScale;
	SetIsDirty();
}

const Vector4<float>& Transform::GetPosition()
{
	return myPosition;
}

const Vector4<float>& Transform::GetRotation()
{
	return myRotation;
}

const Vector4<float>& Transform::GetScale()
{
	return myScale;
}

bool Transform::GetIsDirty()
{
	return myIsDirty;
}

void Transform::SetIsDirty()
{
	myIsDirty = true;
}

void Transform::RemoveIsDirty()
{
	myIsDirty = false;
}
