#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <wrl.h>
#include "Buffers/ConstantBuffer.h"
#include "PipelineStateData.h"

#include "Buffers/FrameBuffer.h"

class Shader;

struct ID3D11InputLayout;
struct ID3D11BlendState1;

class InspectorCamera;
class MeshRenderer;
class SpriteRenderer;

struct Sprite;
struct ID3D11PixelShader;

class Material;
class RenderMeshCommand;
class Renderer;

class RenderHardwareInterface;

namespace TGA { namespace FBX { struct Mesh; } }

enum class ConstantBufferType : unsigned
{
	ObjectBuffer,
	FrameBuffer,
	LightBuffer,
	AnimationBuffer,
	Defaultconstants,
	DebugBuffer,
	PostProcess,
	SpriteEditor,
	SSAO
};

class GraphicsEngine
{
public:
	friend class Transform;

	static GraphicsEngine& Get();
	static const std::unique_ptr<RenderHardwareInterface>& GetRHI();

	void CleanUp();

	bool Init(HWND aWindowHandle);

	template <class BufferData>
	bool UpdateAndSetConstantBuffer(ConstantBufferType aBufferType, const BufferData& aDataBlock);

	void GEChangePipelineState(const PipelineStateObject& aNewPSO);

	FORCEINLINE const PipelineStateObject& GetdefaultPSO() const { 
		return myDefaultPSO; }

	void UpdateRender(float aDeltaTime0);

	void RenderMesh(TGA::FBX::Mesh* aMesh, Material& aMaterial, Microsoft::WRL::ComPtr<ID3D11PixelShader> shaderOverride) const;
	void RenderMeshes(TGA::FBX::Mesh* aMesh, Material& aMaterial, const std::vector<CommonUtilities::Matrix4x4<float>>& aTransforms);

	void Rendersprite(Sprite* aSprite, Material& aMaterial) const;

	void DrawMesh(MeshRenderer* aMeshrenderer);

	Microsoft::WRL::ComPtr<ID3D11BlendState> myBlendstate;

	CommonUtilities::Vector2<float> myGameResolution = CommonUtilities::Vector2<float>();

	FrameBuffer frameBufferData;

	Microsoft::WRL::ComPtr<ID3D11Buffer> myInstanceBuffer;
private:

	std::unique_ptr<RenderHardwareInterface> myRHI;
	std::unordered_map<ConstantBufferType, ConstantBuffer> myConstantBuffer;

	GraphicsEngine();
	~GraphicsEngine();

	PipelineStateObject myDefaultPSO;
	PipelineStateObject myCurrentPSO;

};