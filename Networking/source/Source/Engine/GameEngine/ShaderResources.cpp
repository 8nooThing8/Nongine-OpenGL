#include "../Engine.pch.h"

#include "ShaderResources.h"

#include "../GraphicsEngine/GraphicsEngine.h"

void ShaderResources::CleanUp()
{
	CleanupSRV();
	CleanupSS();
}

void ShaderResources::CleanupSRV()
{
	if (myShaderResourceViews.size())
	{
		delete myShaderResourceViews.back();
		myShaderResourceViews.pop_back();

		CleanupSRV();
	}
}

void ShaderResources::CleanupSS()
{
	if (myShaderSamplerStates.size())
	{
		delete myShaderSamplerStates.front();
		myShaderSamplerStates.pop_back();

		CleanupSS();
	}
}

void ShaderResources::AddShaderResource(ShaderResourceItem* resourceview)
{
	myShaderResourceViews.push_back(resourceview);
}

void ShaderResources::AddSamplerState(SamplerStateItem* samplerState)
{
	myShaderSamplerStates.push_back(samplerState);
}

ShaderResourceItem* ShaderResources::GetNextShaderResource(const int index)
{
	if (index < myShaderResourceViews.size())
	{
		ShaderResourceItem* view = std::move(myShaderResourceViews[index]);

		return view;
	}

	return nullptr;
}

SamplerStateItem* ShaderResources::GetNextSamplerState(const int index)
{
	if (index < myShaderSamplerStates.size())
	{
		SamplerStateItem* sampler = std::move(myShaderSamplerStates[index]);

		return sampler;
	}

	return nullptr;
}

void ShaderResources::SetShaderResourceView(ShaderResourceItem* resource)
{
	GraphicsEngine::GetRHI()->GetContext()->PSSetShaderResources((UINT)resource->slot, (UINT)resource->numViews, resource->resource);
}

void ShaderResources::SetSamplerState(SamplerStateItem* resource)
{
	GraphicsEngine::GetRHI()->GetContext()->PSSetSamplers((UINT)resource->slot, (UINT)resource->numViews, resource->resource);
}

void ShaderResources::SetShaderResources(int index)
{
	ShaderResourceItem* view = GetNextShaderResource(index++);

	if (view)
	{
		SetShaderResourceView(view);

		SetShaderResources(index);
	}
}

void ShaderResources::SetSamplerState(int index)
{
	SamplerStateItem* sampler = GetNextSamplerState(index++);

	if (sampler)
	{
		SetSamplerState(sampler);

		SetSamplerState(index);
	}
}


void ShaderResources::SetShaderObjects()
{
	SetShaderResources(0);
	SetSamplerState(0);
}
