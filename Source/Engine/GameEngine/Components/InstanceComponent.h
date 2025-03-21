#pragma once

#include "Component.h"
#include <Matrix\Matrix.hpp>

#include "../Engine/GraphicsEngine/GraphicsEngine.h"
#include "../Engine/GraphicsEngine/RenderHardwareInterface.h"

#include "../MainSingleton.h"

#include "../ModelSprite/ModelFactory.h"

#include <vector>

#include "../Engine/GraphicsEngine/Material/Material.h"

class MainSingleton;

struct InstanceTransform
{
	InstanceTransform() 
	{
		position = CommonUtilities::Vector4<float>(0, 0, 0, 1);
		rotation = CommonUtilities::Vector4<float>(0, 0, 0, 0);
		scale = CommonUtilities::Vector4<float>(1, 1, 1, 1);
	}

	CommonUtilities::Vector4<float> position;
	CommonUtilities::Vector4<float> rotation;
	CommonUtilities::Vector4<float> scale;

	bool arrowOpen;
};

class InstanceComponent : public Component
{
public:

	InstanceComponent() = default;
	~InstanceComponent() override = default;

	void EarlyUpdateEditor(float aFloat) override;

	void Start() override;

	void RenderImGUI() override;

	std::vector<InstanceTransform> myInstancedModelsTransforms;
	std::vector<CommonUtilities::Matrix4x4<float>> myInstancedModels;

	TGA::FBX::Mesh* instancedMesh;
	Material myInstanceMaterial;

	Material myInstanceMaterialForward;
};