#pragma once

#include <map>
#include <vector>

#include <Primitive/Sphere.hpp>
#include <Primitive/Intersection.hpp>

class Component;

class ComponentManager
{
public:
	ComponentManager() = default;
	~ComponentManager();

	void AddSpritesToGraphicsEngine();
	bool Raycast(const CU::Ray<float>& aRay, float aDistance);
	void DeleteComponents();
	void StartComponents() const;
	void UpdateComponents(float aDeltaTime);
	void EarlyUpdateComponents(float aDeltaTime);

	std::map<std::string, std::vector<Component*>> myComponents;
private:
};
