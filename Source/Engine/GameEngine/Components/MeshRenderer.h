#pragma once

#include "Component.h"
#include "../Engine/GraphicsEngine/Material/Material.h"
#include "../Engine/GraphicsEngine/Mesh.h"

class MeshRenderer : public Component
{
public:
	~MeshRenderer() override = default;
	MeshRenderer() = default;
	MeshRenderer(non::Mesh* aMesh, const Material& aMaterial);

	void Update(float aDeltaTime) override;
	void Start() override;

	void SetMesh(non::Mesh* aMesh);
	void SetMaterial(const Material& aMaterial);

	Material& GetMaterial();
	non::Mesh* GetMesh();
	non::Mesh** GetMeshAdress();

	void SetLayer(int aLayer) { myLayer = aLayer; }
	void SetLayerToRemove(int aLayer) { myLayerToRemove = aLayer; }

	void DrawMesh();

public:
	Material myMaterial;
	non::Mesh* myMesh;

	int myLayer = 0;
	int myLayerToRemove = 0;

	static inline bool addMeshes = true;
};
