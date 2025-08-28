#include "MoveServerObject.h"
#include <InputHandler.h>

#include "../GameObject.h"

#include "Vector/Vector.hpp"

#include <Shared/Common.h>

#include <Timer.h>

MoveServerObject::MoveServerObject() : myCollider(0.048f)
{
}

void MoveServerObject::Start()
{
	myVelocity = { 0, 0 };
	myDrag = 2.f;

	float random0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	float random1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

	myVelocity = CU::Vector2f((random0 * 2 - 1), (random1 * 2 - 1)).GetNormalized() * 0.07f;
}

void MoveServerObject::Update(float aDeltaTime)
{
	CommonUtilities::Vector3f pos(sin(CommonUtilities::Timer::GetTotalTime() * 3.5f) * 0.3f + myVelocity.x, cos(CommonUtilities::Timer::GetTotalTime() * 3.5f) * 0.3f + myVelocity.y, 0.f);
	gameObject->transform->SetPosition(pos);
	
	
	//CU::Vector4f desiredMovement = CU::Vector4f(myVelocity.x, myVelocity.y, 0, 0) * aDeltaTime;
	//gameObject->transform->SetPosition(gameObject->transform->GetPosition() + desiredMovement);
}
