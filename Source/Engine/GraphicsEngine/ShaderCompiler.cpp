#include "../Engine.pch.h"

#include "ShaderCompiler.h"

#include <iostream>
#include <filesystem>
#include <fstream>

#include <GLFW\glfw3.h>
#include <glad\glad.h>


#ifdef OPENGL

bool ShaderCompiler::CompileShader(const char* aPath, unsigned& outShader, ShaderType aType)
{
    switch (aType)
    {
    case ShaderCompiler::FragmentShader:
        outShader = glCreateShader(GL_FRAGMENT_SHADER);
        break;
    case ShaderCompiler::VertexShader:
        outShader = glCreateShader(GL_VERTEX_SHADER);
        break;
    case ShaderCompiler::GeometryShader:
        outShader = glCreateShader(GL_GEOMETRY_SHADER);
        break;
    }

    
    std::ifstream fileStream(aPath, std::ios::binary);

    if (!fileStream.is_open())
        return false;

    fileStream.seekg(0, std::ios::end);
    size_t size = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);

    char* buffer = new char[size + 1];
    fileStream.read(buffer, size);
    fileStream.close();

    buffer[size] = '\0';

    glShaderSource(outShader, 1, &buffer, NULL);
    glCompileShader(outShader);

    delete[] buffer;

    int success;
    char infoLog[512];
    glGetShaderiv(outShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(outShader, 512, NULL, infoLog);
        switch (aType)
        {
        case ShaderCompiler::FragmentShader:
            std::cout << "ERROR::SHADER::FRAGMENT/PIXEL::COMPILATION_FAILED\n" << infoLog << std::endl;
            break;
        case ShaderCompiler::VertexShader:
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
            break;
        case ShaderCompiler::GeometryShader:
            std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
            break;
        }
        
        return false;
    }

    return true;
}

#elif D3D11

#include "GraphicsEngine.h"

#include "Material/Material.h"
#include "d3dcompiler.h"
#include "../GameEngine/Console.h"

#include <fstream>
#include <unordered_map>

#include "Shader.h"

std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID3D11PixelShader>> ourPixelShaders;
std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID3D11VertexShader>> ourVertexShaders;

STDMETHODIMP ShaderIncludeHandler::Open(const D3D_INCLUDE_TYPE IncludeType, const LPCSTR pFileName, const LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
{
    IncludeType;
    pParentData;

    std::string filePath = "";
    std::string shaderIncludesIncludeString(std::string(pFileName).substr(0, 8));

    if (shaderIncludesIncludeString == "Includes")
    {
        filePath = "Assets/Shaders/" + std::string(pFileName);
    }
    else
    {
        filePath = "Assets/Shaders/Includes/" + std::string(pFileName);
    }

    std::ifstream fileStream(filePath, std::ios::binary);

    if (!fileStream.is_open())
        return E_FAIL;

    fileStream.seekg(0, std::ios::end);
    size_t size = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);

    char* buffer = new char[size];
    fileStream.read(buffer, size);
    fileStream.close();

    *ppData = buffer;
    *pBytes = static_cast<UINT>(size);

    return S_OK;
}

STDMETHODIMP ShaderIncludeHandler::Close(const LPCVOID pData)
{
    delete[] static_cast<const char*>(pData);
    return S_OK;
}

Shaders::Shaders() = default;
Shaders::~Shaders() = default;

void Shaders::CompileShader(Material* aMaterial)
{
    HRESULT result;

    std::wstring pathOfPSShader = aMaterial->myPSShader->GetShaderPath();
    std::wstring pathOfVXShader = aMaterial->myVXShader->GetShaderPath();

    Microsoft::WRL::ComPtr<ID3D11PixelShader> psshader;

    if (!ourPixelShaders.contains(pathOfPSShader))
    {
        Microsoft::WRL::ComPtr<ID3DBlob> blob;

        result = CompileShaderFromFile(pathOfPSShader.c_str(), "main", "ps_5_0", blob.GetAddressOf());

        if (FAILED(result))
        {
            PrintError("Error with a pixelshader");
            return;
        }

        result = GraphicsEngine::GetRHI()->GetDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, psshader.GetAddressOf());

        if (FAILED(result))
        {
            std::cout << "it brok";
        }

        std::string pxName = std::filesystem::path(pathOfPSShader).filename().string();
        psshader->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(pxName.length()), pxName.c_str());

        ourPixelShaders[pathOfPSShader] = std::move(psshader);
    }

    aMaterial->myPSShader->SetShader(ourPixelShaders.at(pathOfPSShader));

    Microsoft::WRL::ComPtr<ID3D11VertexShader> vsshader;


    if (!ourVertexShaders.contains(pathOfVXShader))
    {
        Microsoft::WRL::ComPtr<ID3DBlob> newBlob;

        result = CompileShaderFromFile(pathOfVXShader.c_str(), "main", "vs_5_0", newBlob.GetAddressOf());

        if (FAILED(result))
        {
            PrintError("Error with a vertex shader");
            return;
        }

        result = GraphicsEngine::GetRHI()->GetDevice()->CreateVertexShader(newBlob->GetBufferPointer(), newBlob->GetBufferSize(), nullptr, vsshader.GetAddressOf());

        if (FAILED(result))
        {
            PrintError("Error with a vertex shader");
        }

        std::string vxName = std::filesystem::path(pathOfVXShader).filename().string();
        vsshader->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(vxName.length()), vxName.c_str());

        ourVertexShaders[pathOfVXShader] = std::move(vsshader);
    }

    aMaterial->myVXShader->SetShader(ourVertexShaders.at(pathOfVXShader));
}

void Shaders::CompileVertexShader(const std::wstring& aPath, Microsoft::WRL::ComPtr<ID3DBlob>& aBlob)
{
    HRESULT result;

    result = CompileShaderFromFile(aPath.c_str(), "main", "vs_5_0", aBlob.GetAddressOf());

    if (FAILED(result))
    {
        PrintError("Error with a vertexshader");
    }
}

Microsoft::WRL::ComPtr<ID3D11VertexShader> Shaders::CompileVertexShader(const std::wstring& aPath)
{
    HRESULT result;

    Microsoft::WRL::ComPtr<ID3DBlob> blob;

    result = CompileShaderFromFile(aPath.c_str(), "main", "vs_5_0", blob.GetAddressOf());

    if (FAILED(result))
    {
        PrintError("Error compiling vertexshader");
    }

    Microsoft::WRL::ComPtr<ID3D11VertexShader> vxShader;

    result = GraphicsEngine::GetRHI()->GetDevice()->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, vxShader.GetAddressOf());

    if (FAILED(result))
    {
        PrintError("Error creating vertexshader");
    }

    return vxShader;
}

Microsoft::WRL::ComPtr<ID3D11GeometryShader> Shaders::CompileGeometryShader(const std::wstring& aPath)
{
    HRESULT result;

    Microsoft::WRL::ComPtr<ID3DBlob> blob;

    result = CompileShaderFromFile(aPath.c_str(), "main", "gs_5_0", blob.GetAddressOf());

    if (FAILED(result))
    {
        PrintError("Error compiling geometryshader");
    }

    Microsoft::WRL::ComPtr<ID3D11GeometryShader> gsShader;

    result = GraphicsEngine::GetRHI()->GetDevice()->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, gsShader.GetAddressOf());

    if (FAILED(result))
    {
        PrintError("Error creating geometryshader");
    }

    return gsShader;
}

Microsoft::WRL::ComPtr<ID3D11PixelShader> Shaders::CompilePixelShader(const std::wstring& aPath)
{
    HRESULT result;

    Microsoft::WRL::ComPtr<ID3DBlob> blob;

    result = CompileShaderFromFile(aPath.c_str(), "main", "ps_5_0", blob.GetAddressOf());

    if (FAILED(result))
    {
        PrintError("Error compiling pixelshader");
    }

    Microsoft::WRL::ComPtr<ID3D11PixelShader> psshader;

    result = GraphicsEngine::GetRHI()->GetDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, psshader.GetAddressOf());

    if (FAILED(result))
    {
        PrintError("Error compiling pixelshader");
    }

    return psshader;
}

HRESULT Shaders::CompileShaderFromFile(const WCHAR* filePath, const LPCSTR entryPoint, const LPCSTR shaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    dwShaderFlags |= D3DCOMPILE_DEBUG;
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#else
dwShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif
    ID3DBlob* pErrorBlob = nullptr;
    ShaderIncludeHandler includeHandler;

    hr = D3DCompileFromFile(filePath, nullptr, &includeHandler, entryPoint, shaderModel,
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            PrintError(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));

            std::cout << reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer());
            pErrorBlob->Release();
        }
        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

#endif