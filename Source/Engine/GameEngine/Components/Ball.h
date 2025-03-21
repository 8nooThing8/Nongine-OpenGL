#pragma once

#include "Component.h"
#include "../Engine/GameEngine/Components/Physics/Colliders/Collider.h"

#include <iostream>

class Ball : public Component
{
public:
	Ball() = default;
	~Ball() = default;

	void OnCollisionEnter(GameObject* aGo, const CU::Vector3f* contactPoint) override
	{
		if (!ballHitWall)
		{
			if (aGo->GetComponent<Collider>(true))
			{
				if (aGo->GetComponent<Collider>(true)->GetLayer() == CollisionLayer::Cabin)
				{
					physx::PxU32 layer = static_cast<physx::PxU32>(CollisionLayer::Cabin);
					Hits hits;

					bool rayHit = PhysicsEngine::RayCasts(CU::Ray<float>(gameObject->transform->position, CU::Vector4f(*contactPoint) - gameObject->transform->position), hits, 99999999.f, layer);

					if (rayHit)
					{
						auto go = MainSingleton::Get()->activeScene->NewEmptyGameObject("House boolean");

						go->transform->position = CU::Vector4f(hits.hits[0].position);

						if (hits.hits[0].localNormal.z > 0)
						{
							go->transform->position.x -= hits.hits[0].normal.x * 5.f;
							go->transform->position.y -= hits.hits[0].normal.y * 5.f;
							go->transform->position.z -= hits.hits[0].normal.z * 5.f;
						}

						go->transform->rotation = hits.hits[0].hitObject->transform->rotation;
						go->transform->scale = CommonUtilities::Vector4f(6, 6, 3.f, 1);

						MeshRenderer* meshRend = go->AddComponent<MeshRenderer>();
						meshRend->myMesh = CreateModel("Assets/Models/Cylinder.fbx");
						meshRend->SetDeffered(true);
						meshRend->GetMaterial().SetColor(CommonUtilities::Vector4f(0.f, 0.f, 0.f, 0.f));
						meshRend->RemoveLayer(hits.hits[0].hitObject->GetComponent<MeshRenderer>()->myLayer);

						auto collider = go->AddComponent<MeshCollider>();
						collider->SetMesh(CreateModel("Assets/Models/Cylinder.fbx"));

						//auto collider = go->AddComponent<SphereCollider>();
						//collider->SetRadius(8);

						collider->SetOutsideShape();
						collider->AddIgnoreCollider(collider);

						go->StartComponents();

						gameObject->GetComponent<RigidBody>()->SetVelocity(-hits.hits[0].normal * 100.f);
					}

					ballHitWall = true;
				}
			}
		}
	}

private:
	bool ballHitWall = false;
};