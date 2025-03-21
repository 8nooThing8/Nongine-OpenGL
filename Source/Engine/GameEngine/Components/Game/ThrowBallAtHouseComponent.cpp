#include "ThrowBallAtHouseComponent.h"

#include "../GameObject.h"

#include <InputHandler.h>

#include "../Engine/GameEngine/GameCamera.h"
#include "../PhysicsEngine/PhysicsEngine.h"

#include "../Engine/GameEngine/Components/MeshRenderer.h"

#include "../Engine/GameEngine/SceneManagerInternal.h"
#include "../Physics/Colliders/MeshCollider.h"
#include "../Physics/Colliders/SphereCollider.h"
#include "../Physics/RigidBody.h"
#include <../Engine/GameEngine/Components/Ball.h>

void ThrowBallAtHouseComponent::Start()
{

}

void ThrowBallAtHouseComponent::Update(float aDeltaTime)
{
	if (InputHandler::GetInstance()->GetMouseButton(1))
	{
		float rotX = 0;
		float rotY = 0;

		rotX = InputHandler::GetInstance()->GetMouseDelta().y * aDeltaTime;
		rotY = InputHandler::GetInstance()->GetMouseDelta().x * aDeltaTime;

		gameObject->transform->rotation.x = std::clamp(gameObject->transform->rotation.x + rotX * 80.f, -90.f, 90.f);
		gameObject->transform->rotation.y = gameObject->transform->rotation.y + rotY * 80.f;

		bool shifting = InputHandler::GetInstance()->GetKey(keycode::SHIFT);

		const int fwAmount = InputHandler::GetInstance()->GetKey(keycode::W) - InputHandler::GetInstance()->GetKey(keycode::S);
		const int lrAmount = InputHandler::GetInstance()->GetKey(keycode::D) - InputHandler::GetInstance()->GetKey(keycode::A);
		const int udAmount = InputHandler::GetInstance()->GetKey(keycode::SPACE) - InputHandler::GetInstance()->GetKey(keycode::C);

		bool tra = InputHandler::GetInstance()->GetKey(keycode::CONTROL);

		CommonUtilities::Vector3<float> movement = CommonUtilities::Vector3<float>(
			static_cast<float>(fwAmount) * gameObject->transform->GetForward() +
			static_cast<float>(lrAmount) * gameObject->transform->GetRight() +
			static_cast<float>(udAmount) * gameObject->transform->GetUp()) *
			aDeltaTime * 25.f * (shifting ? 5 : tra ? 0.1f : 1.f);

		gameObject->transform->position += movement;
	}

	if (InputHandler::GetInstance()->GetMouseButtonDown(0))
	{
		{
			auto go = MainSingleton::Get()->activeScene->NewEmptyGameObject("Metal Ball");

			go->transform->position = gameObject->transform->position;
			go->transform->scale = CommonUtilities::Vector4f(5.f, 5.f, 5.f, 1);

			MeshRenderer* meshRend = go->AddComponent<MeshRenderer>();
			meshRend->myMesh = CreateModel("Assets/Models/UVSphere.fbx");
			meshRend->SetDeffered(true);

			meshRend->GetMaterial().SetColor(CommonUtilities::Vector4f(1.f, 1.f, 1.f, 1.f));

			go->AddComponent<Ball>();

			auto rigidbody = go->AddComponent<RigidBody>();

			auto collider = go->AddComponent<SphereCollider>();
			collider->SetRadius(go->transform->scale.x);

			go->StartComponents();

			rigidbody->SetVelocity(GameCamera::main->myForward * 1000.f);
		}
	}
}