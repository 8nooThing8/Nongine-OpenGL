#pragma once

#include <unordered_map>
#include <vector>

#include <string>
#include <wrl.h>

class Component;
class GameObject;

class MeshRenderer;
class SpriteRenderer;
class ReflectionProbe;
struct ID3D11PixelShader;

class ComponentManager
{
public:
	ComponentManager() = default;
	~ComponentManager();

	void CleanUp();

	void StartComponents() const;
	void StopComponents() const;

	void UpdateComponents(float aDeltaTime);
	void UpdateComponentsEarly(float aDeltaTime);
	void UpdateComponentsLate(float aDeltaTime);

	void EditorUpdateComponents(float aDeltaTime);
	void EditorUpdateComponentsEarly(float aDeltaTime);
	void EditorUpdateComponentsLate(float aDeltaTime);

	void RenderMeshOverride(MeshRenderer* notRenderThis = nullptr, Microsoft::WRL::ComPtr<ID3D11PixelShader> shaderOverride = nullptr);
	void RenderMeshesOnLayer(int aLayer = 1);
	void RenderRemoveMeshesLayer(int aLayer = 1);

	std::vector<GameObject*> GetGameObjectsOnLayer(int aLayer);

	void RenderSprite(SpriteRenderer* notRenderThis = nullptr);

	void LightOverride();

	void ReflectionOverride(ReflectionProbe* aProbe, float count);

	std::unordered_map<std::string, std::vector<Component*>> myComponents;
private:
};