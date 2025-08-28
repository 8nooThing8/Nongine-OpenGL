#pragma once

#include "Component.h"
#include "../Engine/GameEngine/Components/Physics/Colliders/Collider.h"
#include "../Engine/GameEngine/Components/Physics/RigidBody.h"
#include "../Engine/GameEngine/Components/GameObject.h"

#include <iostream>
#include "MeshRenderer.h"
#include "Physics\Colliders\MeshCollider.h"
#include "..\ModelSprite\ModelFactory.h"

class Ball : public Component
{
public:
	Ball() : material("Assets/Shaders/DefaultFS.glsl", "Assets/Shaders/DefaultVS.glsl")
	{
		
	}
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

					bool rayHit = PhysicsEngine::RayCasts(CU::Ray<float>(gameObject->transform->GetPosition(), CU::Vector4f(*contactPoint) - gameObject->transform->GetPosition()), hits, 99999999.f, layer);

					if (rayHit)
					{
						auto go = new GameObject("House boolean");

						go->transform->SetPosition(hits.hits[0].position);

						if (hits.hits[0].hitObject->GetName() == "CabinRoof")
						{
							float x = hits.hits[0].normal.x * -4.f;
							float y = hits.hits[0].normal.y * -4.f;
							float z = hits.hits[0].normal.z * -4.f;

							if (hits.hits[0].localNormal.z > 0)
							{
								x = hits.hits[0].normal.x * 8.f;
								y = hits.hits[0].normal.y * 8.f;
								z = hits.hits[0].normal.z * 8.f;
							}

							const CU::Vector3f& postition = go->transform->GetPosition();
							go->transform->SetPosition({ postition.x - x, postition.y - y, postition.z - z });
						}

						else if (hits.hits[0].localNormal.z > 0)
						{
							float x = hits.hits[0].normal.x * 5.f;
							float y = hits.hits[0].normal.y * 5.f;
							float z = hits.hits[0].normal.z * 5.f;

							const CU::Vector3f& postition = go->transform->GetPosition();

							go->transform->SetPosition({ postition.x - x, postition.y - y, postition.z - z });
						}

						go->transform->SetRotation(hits.hits[0].hitObject->transform->GetRotation());
						go->transform->SetScale({ 6, 6, 3.f });

						MeshRenderer* meshRend = go->AddComponent<MeshRenderer>(CreateModel("Assets/Models/Cylinder.fbx"), material);
						meshRend->GetMaterial().SetColor(CommonUtilities::Vector4f(0.f, 0.f, 0.f, 1.f));
						meshRend->SetLayerToRemove(hits.hits[0].hitObject->GetComponent<MeshRenderer>()->myLayer);
						meshRend->DrawMesh();

						auto collider = go->AddComponent<MeshCollider>();
						collider->SetMesh(CreateModel("Assets/Models/Cylinder.fbx"), 1.5f);

						//auto collider = go->AddComponent<SphereCollider>();
						//collider->SetRadius(8);

						collider->SetOutsideShape();
						hits.hits[0].hitObject->GetComponent<Collider>(true)->AddIgnoreCollider(collider);

						go->StartMyComponents();

						gameObject->GetComponent<RigidBody>()->SetVelocity(-hits.hits[0].normal * 100.f);
					}

					ballHitWall = true;
				}
			}
		}
	}

private:
	Material material;

	bool ballHitWall = false;
};