#pragma once
#include "../Component.h"

#include <memory>
#include <vector>

#include "../../Camera.h"
#include "../../GameCamera.h"

#include "TrailPoint.h"
#include "TrailRenderer.h"

class Trail;

struct TrailSorting
{
	bool operator() (TrailPoint* pa1, TrailPoint* pa2) const
	{
		float distanceToCamera1 = (pa1->position - GameCamera::main->position).Length();
		float distanceToCamera2 = (pa2->position - GameCamera::main->position).Length();

		return distanceToCamera1 > distanceToCamera2;
	}
};

class TrailSystem : public Component
{
	enum TrailVisulizerShape
	{
		Wave,
		Circle,
		Line
	};

public:
	friend class Trail;

	void SpawnTrailPoint
	(
		const CommonUtilities::Vector4<float>& aPosition,
		const CommonUtilities::Vector4<float>& aRotation,
		float aThickness,
		const CommonUtilities::Vector4<float>& aColor,
		float aLifeTime
	);

	void Start() override;
	void LateUpdateEditor(float) override;
	std::string ShapeToString(TrailVisulizerShape aShape);

	void RenderImGUI() override;

private:
	std::vector<Trail*> myTrails;

	std::shared_ptr<TrailRenderer> myTrailRenderer;

	bool playing = false;
	bool paused = false;

	float centerZ;
	float centerY;

	float angle = 0.0f;
	float radius = 60.0f;
	float speed = 6.0f;

	TrailVisulizerShape shape = Circle;

	//CommonUtilities::Vector4<>
};

