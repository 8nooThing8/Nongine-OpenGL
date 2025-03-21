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

	void SetDeffered(const bool aDeffered);

	void SetCastShadow(const bool aValue);

	void LateUpdateEditor(float /*aDeltaTime*/) override;

	void ShaderCleanUp() const;

	void Draw(Microsoft::WRL::ComPtr<ID3D11PixelShader> shaderOverride = nullptr);

	void Destroy();

	void RenderImGUI() override;

	TGA::FBX::Mesh* GetMesh() const;

	Material& GetMaterial();

public:
	char PXshaderPath[256] = "";
	char VXshaderPath[256] = "";

	ShaderResources* shaderItems;

	Material myMaterial;
	TGA::FBX::Mesh* myMesh = nullptr;

	bool myCastShadow;

	bool myDeffered;
};
