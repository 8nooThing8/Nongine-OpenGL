#pragma once

#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_GEOMETRY_SHADER 0x8DD9

#include <string>

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