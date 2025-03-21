#pragma once

#include <wrl.h>

#include "RHIStruct.h"
#include "PipelineStateData.h"

#include "Matrix/Matrix4x4.hpp"

#include "Material/Material.h"

class Shader;
class Texture;
struct D3D11_VIEWPORT;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11Buffer;
struct ID3D11InputLayout;
struct ID3D11ShaderResourceView;
struct ID3D11Resource;
struct ID3D11SamplerState;

struct Sprite;

struct ID3D11Texture2D;

class ConstantBuffer;

enum class UseTexture
{
	Diffuse = 1,
	Normal = 2,
	Roughness = 4,
	Metallic = 8,
	AO = 16,
};

class RenderHardwareInterface
{
public:
	RenderHardwareInterface() = default;
	~RenderHardwareInterface() = default;

	bool Init(HWND aHWND, bool aEnableDebug);
	CommonUtilities::Vector4<float> GetPixelDX11(int X, int Y, ID3D11ShaderResourceView* SourceSRV);
	void Present() const;

	template <class VertexType>
	bool CreateVertexBuffer(std::string_view aName, const std::vector<VertexType>& aVertexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer, bool aEditable = false);
	bool CreateIndexBuffer(std::string_view aName, const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outIndexBuffer, bool aEditable = false);
	bool CreateConstantBuffer(std::string_view aName, size_t aSize, unsigned aSlot, unsigned aPipeLineStages, ConstantBuffer& outConstantBuffer);

	template <class VertexType>
	bool UpdateVertexBuffer(const std::vector<VertexType>& aVertexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer);

	bool UpdateIndexBuffer(const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& aVxBuffer);


	void SetVertexBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aVertexBuffer, size_t aVertexSize, size_t aVertexOffset = 0);
	void SetIndexBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aIndexBuffer) const;

	void SetVertexBuffers(const std::vector<ID3D11Buffer*>& aVertexBuffer, const std::vector < size_t>& aVertexSize, const std::vector < size_t>& aVertexOffset = {0});
	void SetVertexBuffers(const std::vector<ID3D11Buffer*>& aVertexBuffer, const std::vector < unsigned>& aVertexSize, const std::vector < unsigned>& aVertexOffset = {0});

	void SetConstantBuffer(const ConstantBuffer& aBuffer);

	template <class BufferData>
	bool UpdateConstantBuffer(const ConstantBuffer& abuffer, const BufferData& aBufferData);
	
	void SetPrimitiveTopology(unsigned aTopology) const;

	bool CreateInputLayout(Microsoft::WRL::ComPtr<ID3D11InputLayout>& outInputLayout, const std::vector<VertexElementDesc>& aInputLayoutDefinition, const void* vsBytecode, size_t vsBytecodeSize);
	void SetInputLayout(Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);

	bool LoadShaderFromMemory(std::string_view aName, Shader& outShader, const uint8_t* aShaderDataPtr, size_t aShaderDataSize);

	bool LoadPSFromFile(std::wstring shaderName, Shader& outShader, const std::wstring& shaderPath);
	bool LoadVSFromFile(std::wstring shaderName, Shader& outShader, const std::wstring& shaderPath, std::string& data);

	void Draw(unsigned aVertexCount);

	void DrawIndexedTexture(unsigned aStartIndex, unsigned aIndexCount, unsigned aVertexOffset, const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aTextrure) const;
	void DrawVertexTexture(unsigned aVertexCount, const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aTextrure) const;

	void DrawInstanced(unsigned aIndexCount, unsigned aInstanceCount, unsigned aIndexOffset, unsigned aVertexOffset, Material& aMaterial) const;

	void DrawIndexed(unsigned aStartIndex, unsigned aIndexCount, unsigned aVertexOffset, Material& aMaterial) const;
	void DrawTextured(const unsigned aVertexCount, Material& aMaterial) const;

	void DrawIndexedSprite(unsigned aStartIndex, unsigned aIndexCount, unsigned aVertexOffset, Sprite* aSprite) const;

	void ChangePipelineState(const PipelineStateObject& aNewPSO, const PipelineStateObject& aOldPSO);

	Microsoft::WRL::ComPtr<ID3D11Device>& GetDevice();
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GetContext();

	void ClearBackBuffer() const;
	void ClearSceneBuffer() const;
	void ClearPPBuffer() const;
	void ClearDefferedBuffer() const;

	void SetGeometryShader(const Shader& aGxShader);

	void ResetGeometryShader();

	void SetShaders(const Shader& aVxShader, const Shader& aPxShader);
	void SetShaders(const Shader& aVxShader, const Microsoft::WRL::ComPtr<ID3D11PixelShader> pxShader);
	void ClearRenderTargetView(Microsoft::WRL::ComPtr<ID3D11RenderTargetView> aRTV) const;
	void SetRenderTargetView(Microsoft::WRL::ComPtr<ID3D11RenderTargetView> aRTV);
	void SetRenderTargetView(Microsoft::WRL::ComPtr<ID3D11RenderTargetView> aRTV, D3D11_VIEWPORT aViewPort);
	void SetBackBufferRT();
	void SetSceneBufferRT(bool depthBuffer = false);

	void SetDefferedBufferRT();
	void SetPostProcessBufferRT(bool useDepth = false);


	Microsoft::WRL::ComPtr<ID3D11SamplerState> sSamplerState;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> ImGuiRenderTargetTexture;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rsPt;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rsNoCulling;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> myGetPixelTexture;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthTexture;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthTextureScene;

	std::shared_ptr<Texture> myBackBuffer;

	std::shared_ptr<Texture> mySceneBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mySceneSRV;

	std::shared_ptr<Texture> myPostProcessBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myPostProcessSRV;


	std::shared_ptr<Texture> myDefferedAlbedoBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myDefferedAlbedoSRV;

	std::shared_ptr<Texture> myDefferedNormalBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myDefferedNormalSRV;

	std::shared_ptr<Texture> myDefferedPositionBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myDefferedPositionSRV;

	std::shared_ptr<Texture> myDefferedMaterialBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myDefferedMaterialSRV;

	std::shared_ptr<Texture> myDefferedPixelPickerBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myDefferedPixelPickerSRV;


	static inline Microsoft::WRL::ComPtr<ID3D11Device> myDevice = nullptr;
	static inline Microsoft::WRL::ComPtr<ID3D11DeviceContext> myContext = nullptr;
	static inline Microsoft::WRL::ComPtr<IDXGISwapChain> mySwapChain = nullptr;

private:
	bool CreateVertexBufferInternal(std::string_view aName, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer, const uint8_t* aVertexDataPointer, size_t aNumVerticies, size_t aVertexSize, bool aEditable);

	bool UpdateVertexBufferInternal(
		Microsoft::WRL::ComPtr<ID3D11Buffer>& aVxBuffer,
		const void* aBufferData,
		const size_t aSourceSize
	);

	bool UpdateIndexBufferInternal(
		Microsoft::WRL::ComPtr<ID3D11Buffer>& aIdxBuffer,
		const void* aBufferData,
		const size_t aNumIndicies
	);

	bool UpdateConstantBufferInternal(const ConstantBuffer& aBuffer, const void* aBufferData, size_t aBufferdataSize);

	//template <class VertexType>
	//void SetObjectName(Microsoft::WRL::ComPtr<ID3D11DeviceChild> aObject, const std::vector<VertexType>& aVertexList);
};

template<class VertexType>
inline bool RenderHardwareInterface::CreateVertexBuffer(const std::string_view aName, const std::vector<VertexType>& aVertexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer, bool aEditable)
{
	const size_t vxSize = sizeof(VertexType);
	const size_t vxCount = aVertexList.capacity();

	return CreateVertexBufferInternal(aName, outVxBuffer, reinterpret_cast<const uint8_t*>(aVertexList.data()), vxCount, vxSize, aEditable);
}

template<class BufferData>
inline bool RenderHardwareInterface::UpdateConstantBuffer(const ConstantBuffer& aBuffer, const BufferData& aBufferData)
{
	const size_t dataSize = sizeof(BufferData);
	return UpdateConstantBufferInternal(aBuffer, &aBufferData, dataSize);
}
