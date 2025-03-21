#pragma once

#include "vector"

struct ID3D11ShaderResourceView;
struct ID3D11SamplerState;

struct ShaderResourceItem
{
	int slot;
	size_t numViews;
	ID3D11ShaderResourceView* const* resource;

	ShaderResourceItem(const int aSlot, const size_t aNumViews, ID3D11ShaderResourceView* const* aResource) : slot(aSlot), numViews(aNumViews), resource(aResource)
	{

	}
};

struct SamplerStateItem
{
	int slot;
	size_t numViews;
	ID3D11SamplerState* const* resource;

	SamplerStateItem(const int aSlot, const size_t aNumViews, ID3D11SamplerState* const* aResource) : slot(aSlot), numViews(aNumViews), resource(aResource)
	{

	}
};

class ShaderResources
{
public:
	ShaderResources() = default;
	~ShaderResources() = default;

	void CleanUp();

	void CleanupSRV();
	void CleanupSS();

	void AddShaderResource(ShaderResourceItem* resourceview);
	void AddSamplerState(SamplerStateItem* resourceview);

	ShaderResourceItem* GetNextShaderResource(int index);
	SamplerStateItem* GetNextSamplerState(int index);

	void SetShaderResources(int index);
	void SetSamplerState(int index);

	void SetShaderResourceView(ShaderResourceItem* resource);
	void SetSamplerState(SamplerStateItem* resource);

	void SetShaderObjects();

private:
	std::vector<ShaderResourceItem*> myShaderResourceViews;
	std::vector<SamplerStateItem*> myShaderSamplerStates;
};