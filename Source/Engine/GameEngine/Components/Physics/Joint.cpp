#include "GameEngine.pch.h"
#include "Engine\Components\Physics\Joint.h"
#include "physx\PxSimulationEventCallback.h"

namespace physx
{
	PxU32 myCustomSolverPrep(
		Px1DConstraint* /*constraints*/,
		PxVec3p& /*bodyAWorldOffset*/,
		PxU32 /*maxConstraints*/,
		PxConstraintInvMassScale& /*invMassScale*/,
		const void* /*constantBlock*/,
		const PxTransform& /*bodyAToWorld*/,
		const PxTransform& /*bodyBToWorld*/,
		bool /*useExtendedLimits*/,
		PxVec3p& /*cAtW*/,
		PxVec3p& /*cBtW*/)
	{
		//// Calculate the vector between the bodies
		//PxVec3 chainVector = bodyBToWorld.p - bodyAToWorld.p;
		//float currentLength = chainVector.magnitude();

		//// Normalize the chain vector
		//PxVec3 chainDir = chainVector.getNormalized();

		//// Set up the distance constraint
		//Px1DConstraint& distanceConstraint = constraints[0];
		//distanceConstraint.linear0 = chainDir;
		//distanceConstraint.linear1 = -chainDir;
		//distanceConstraint.angular0 = PxVec3(0);
		//distanceConstraint.angular1 = PxVec3(0);

		//// Set the maximum allowed distance
		//float maxDistance = 1.0f;  // Adjust this to your desired maximum distance

		//// Calculate the geometric error (only if we're exceeding the max distance)
		//float error = currentLength - maxDistance;
		//distanceConstraint.geometricError = error;

		//// Set constraint flags for a unilateral constraint (only active when stretched)
		//distanceConstraint.flags = Px1DConstraintFlag::eOUTPUT_FORCE;

		//// Set solver coefficients
		//distanceConstraint.solveHint = PxConstraintSolveHint::eEQUALITY;

		//// Only apply force to pull bodies together, not push them apart
		//distanceConstraint.minImpulse = -PX_MAX_F32;
		//distanceConstraint.maxImpulse = PX_MAX_F32;

		//// Update the body offsets
		//bodyAWorldOffset = cAtW = bodyAToWorld.p;
		//cBtW = bodyBToWorld.p;
		//cAtW = bodyAToWorld.p;

		return 1;  // We've set up one constraint
	}
}

PxConstraintSolverPrep TreRaderKod::MyConstraintConnector::getPrep() const
{
	return myCustomSolverPrep;
}

// A size is radius for spheres and box extends
TreRaderKod::JointConnector::JointConnector(physx::PxRigidActor* actor1, physx::PxRigidActor* actor2) : myActor1(actor1), myActor2(actor2)
{
	Component::Component();

	constraintConnector = MyConstraintConnector(actor1, actor2);

	PxConstraintShaderTable constraintShaderTable;

	constraintShaderTable.solverPrep = constraintConnector.getPrep();

	PxU32 dataSize = sizeof(MyConstraintConnector);

	PxConstraint* joint = PhysicsEngine::Get().NewJoint(actor1, actor2, constraintConnector, constraintShaderTable, dataSize);

	joint;
}

TreRaderKod::JointConnector::~JointConnector()
{

}

void TreRaderKod::JointConnector::Start()
{

}

void TreRaderKod::JointConnector::Update(float /*aDeltaTime*/)
{
	//UpdateDynamic();
}

template<class T>
void TreRaderKod::JointConnector::UpdateTransform()
{
	/*physx::PxTransform tm = static_cast<T*>(myBody)->getGlobalPose();

	CommonUtilities::Vector3<float> position = gameObject->GetTransform().GetPosition();
	CommonUtilities::Vector4<float> rotation = gameObject->GetTransform().GetRotationMatrix().GetInverse().GetRotationAsQuerternion();

	tm.p = physx::PxVec3(position);
	tm.q = physx::PxQuat(rotation);

	myBody->is<T>()->setGlobalPose(tm);*/
}

void TreRaderKod::JointConnector::UpdateDynamic()
{
	//if (gameObject->GetTransform().GetIsDirty())
	//	UpdateTransform<physx::PxRigidStatic>();
	//else
	//{
	//	PxRigidDynamic* myDynamicBody = myBody->is<PxRigidDynamic>();

	//	if (CU::InputManager::GetInstance().IsKeyDown(CommonUtilities::Keys::UP))
	//	{
	//		myDynamicBody->setLinearVelocity(PxVec3(myDynamicBody->getLinearVelocity().x, myDynamicBody->getLinearVelocity().y, 100));
	//	}
	//	if (CU::InputManager::GetInstance().IsKeyDown(CommonUtilities::Keys::DOWN))
	//	{
	//		myDynamicBody->setLinearVelocity(PxVec3(myDynamicBody->getLinearVelocity().x, myDynamicBody->getLinearVelocity().y, -100));
	//	}

	//	if (CU::InputManager::GetInstance().IsKeyDown(CommonUtilities::Keys::RIGHT))
	//	{
	//		myDynamicBody->setLinearVelocity(PxVec3(100, myDynamicBody->getLinearVelocity().y, myDynamicBody->getLinearVelocity().z));
	//	}
	//	if (CU::InputManager::GetInstance().IsKeyDown(CommonUtilities::Keys::LEFT))
	//	{
	//		myDynamicBody->setLinearVelocity(PxVec3(-100, myDynamicBody->getLinearVelocity().y, myDynamicBody->getLinearVelocity().z));
	//	}

	//	if (CU::InputManager::GetInstance().IsKeyDown(CommonUtilities::Keys::Q))
	//	{
	//		myDynamicBody->setLinearVelocity(PxVec3(myDynamicBody->getLinearVelocity().x, 250.f, myDynamicBody->getLinearVelocity().z));
	//	}

	//	CommonUtilities::Vector3<float> offsetRightAmount = myOffset.x * gameObject->GetTransform().LocalRight();
	//	CommonUtilities::Vector3<float> offsetUpAmount = myOffset.y * gameObject->GetTransform().LocalUp();
	//	CommonUtilities::Vector3<float> offsetForwardAmount = myOffset.z * gameObject->GetTransform().LocalForward();

	//	gameObject->GetTransform().SetPosition(myDynamicBody->getGlobalPose().p - physx::PxVec3(offsetRightAmount + offsetUpAmount + offsetForwardAmount));

	//	physx::PxQuat rotation = myDynamicBody->getGlobalPose().q;

	//	gameObject->GetTransform().SetRotationQuerternion(CommonUtilities::Vector4<float>(rotation.x, rotation.y, rotation.z, rotation.w));
	//}
}