#pragma once

#include "Component.h"
#include "GameObject.h"

#include "Transform.h"

class CharacterController;

class Move : public Component
{
public:
	friend class MeshRenderer;
	friend class RenderMeshCommand;

	Move() = default;
	~Move() = default;

	void Update(const float aDeltaTime) override;

	void Start() override;

private:
	float moveSpeed = 1000.f;

};

