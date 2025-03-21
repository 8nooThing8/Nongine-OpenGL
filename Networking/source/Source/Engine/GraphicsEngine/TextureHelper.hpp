#pragma once

#include <memory>
#include <wrl.h>

class GameObject;
class SpriteRenderer;
class MeshRenderer;
class ShaderResources;

struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;

struct ShaderResourceItem;
struct D3D11_TEXTURE2D_DESC;

class TextureHelper
{
public:
	TextureHelper() = default;
	~TextureHelper() = default;

	static void Init();


	static void AddTextureResource(int aSlot, size_t aNumViews, ID3D11ShaderResourceView* const* aResource);

	static void CreateTexture(const std::wstring& aShaderToUse, int aSlot, ID3D11ShaderResourceView* aResourceIn, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aResourceOut);
	static void CreateTexture3D(const std::wstring& aShaderToUse, int aSlot, ID3D11ShaderResourceView* aResourceIn, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aResourceOut);

private: 
	static void InitTextures(D3D11_TEXTURE2D_DESC aTextureDesc);

	static inline TextureHelper* Instance = nullptr;
	std::shared_ptr<ShaderResources> myResources = nullptr;
	std::shared_ptr<GameObject> myGO;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
	SpriteRenderer* mySpriteRenderer;
	MeshRenderer* myMeshRenderer;

	D3D11_TEXTURE2D_DESC myTextureDesc;
};
