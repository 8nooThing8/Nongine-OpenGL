#pragma once

#include <string>
#include <wrl/client.h>

#include <d3d11.h>

// Forward declarations
struct ID3D11Device;
struct ID3D11PixelShader;
struct ID3D11VertexShader;
//struct ID3D10Blob;
//struct ID3DInclude;
class Material;

struct ShaderIncludeHandler : public ID3DInclude
{
public:
    STDMETHOD(Open)(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) override;
    STDMETHOD(Close)(LPCVOID pData) override;
};

class Shaders
{
public:
    Shaders();
    ~Shaders();

    static void CompileShader(Material* aMaterial);
    static void CompileVertexShader(const std::wstring& aPath, Microsoft::WRL::ComPtr<ID3D10Blob>& aBlob);
    static Microsoft::WRL::ComPtr<ID3D11VertexShader> CompileVertexShader(const std::wstring& aPath);
    static Microsoft::WRL::ComPtr<ID3D11GeometryShader> CompileGeometryShader(const std::wstring& aPath);
    static Microsoft::WRL::ComPtr<ID3D11PixelShader> CompilePixelShader(const std::wstring& aPath);
    static HRESULT CompileShaderFromFile(const WCHAR* filePath, LPCSTR entryPoint, LPCSTR shaderModel, ID3D10Blob** ppBlobOut);
};
