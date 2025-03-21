#pragma once

#include "../GameEngine/Engine/Components/Component.h"

#include "PxPhysics.h"
#include "physx\PxSimulationEventCallback.h"

using namespace physx;

namespace TreRaderKod
{
    class MyConstraintConnector : public PxConstraintConnector
    {
    public:
        PxRigidActor* actor0;
        PxRigidActor* actor1;
        PxConstraint* constraint;

        MyConstraintConnector(PxRigidActor* a0, PxRigidActor* a1) : actor0(a0), actor1(a1), constraint(nullptr)
        {

        }

        MyConstraintConnector() = default;

        void* prepareData() override
        {
            // Prepare the constraint data (this will vary based on your implementation)
            return this;
        }

        bool updatePvdProperties(physx::pvdsdk::PvdDataStream& /*pvdConnection*/, const PxConstraint* /*c*/, PxPvdUpdateType::Enum /*updateType*/) const override
        {
            // Prepare the constraint data (this will vary based on your implementation)
            return true;
        }

        void updateOmniPvdProperties() const override
        {
            
        }

        void onConstraintRelease() override
        {
            // Handle the constraint destruction event
            delete this;
        }

        void onComShift(PxU32 /*actor*/) override
        {

        }

        PxBase* getSerializable() override
        {
            return nullptr;
        }

        PxConstraintSolverPrep getPrep() const override;

        void* getExternalReference(PxU32& /*typeID*/) override
        {
            //typeID = PxConstraintExtIDs::eUSER_DEFINED;  // Custom ID for user-defined constraints
            return this;
        }

        void onOriginShift(const PxVec3& /*shift*/) override
        {
            
        }

        virtual const void* getConstantBlock()	const override
        {
            return this;
        };

    };

	class JointConnector : public Component
	{
	public:
		JointConnector(physx::PxRigidActor* actor1, physx::PxRigidActor* actor2);

        JointConnector() = default;

		~JointConnector() override;

		void Start() override;
		void Update(float aDeltaTime) override;

	private:
		template<class T>
		void UpdateTransform();

		void UpdateDynamic();

		physx::PxRigidActor* myActor1;
		physx::PxRigidActor* myActor2;

        MyConstraintConnector constraintConnector;
	};
}