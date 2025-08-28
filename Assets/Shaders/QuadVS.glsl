#version 460 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 vtxTexCoord;

void main()
{
    vec2 ndcPos = aPos * 2 - 1;

    gl_Position = vec4(ndcPos, 0, 1);

    vtxTexCoord = aTexCoord;
}