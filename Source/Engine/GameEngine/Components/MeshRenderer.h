#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "../Engine/GameEngine/GraphicCommands/RenderMeshCommand.h"
#include "../Engine/GraphicsEngine/Material/Material.h"
#include "Component.h"

#include <wrl.h>

using namespace CommonUtilities;
namespace fs = std::filesystem;

class ShaderResources;

class MeshRenderer : public Component
{
public:
	friend class GraphicsEngine;

	~MeshRenderer() override = default;

	MeshRenderer();

	MeshRenderer(const MeshRenderer& aModel);

	MeshRenderer(TGA::FBX::Mesh* aMesh, Material aMaterial);

	void SetLayer(int aLayer);
	void RemoveLayer(int aLayer);

	void SetDeffered(bool aDeffered);

	void SetCastShadow(bool aValue);

	void LateUpdateEditor(float /*aDeltaTime*/) override;

	void ShaderCleanUp() const;

	void Draw(Microsoft::WRL::ComPtr<ID3D11PixelShader> shaderOverride = nullptr);

	void Destroy();

	void RenderImGUI() override;

	TGA::FBX::Mesh* GetMesh() const;

	Material& GetMaterial();

	void HandleSave(rapidjson::Value& outJsonObj, rapidjson::Document& aDocument) override;
	void HandleLoad(const rapidjson::Value& outJsonObj) override;

	int myLayer = 0;

	int myLayerToRemove = 0;

public:
	char PXshaderPath[256] = "";
	char VXshaderPath[256] = "";

	ShaderResources* shaderItems;

	Material myMaterial;
	TGA::FBX::Mesh* myMesh = nullptr;

	bool myCastShadow;

	bool myDeffered;
};
