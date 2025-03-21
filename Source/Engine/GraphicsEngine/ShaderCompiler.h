#pragma once

#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_GEOMETRY_SHADER 0x8DD9

#include <string>

#ifdef OPENGL

class ShaderCompiler
{
public:
    enum ShaderType
    {
        FragmentShader = GL_FRAGMENT_SHADER,
        PixelShader = GL_FRAGMENT_SHADER,
        VertexShader = GL_VERTEX_SHADER,
        GeometryShader = GL_GEOMETRY_SHADER
    };

    ShaderCompiler() = default;
    ~ShaderCompiler() = default;

    static bool CompileShader(const char* aPath, unsigned& outShader, ShaderType aType);
};

#elif D3D11

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
#endif