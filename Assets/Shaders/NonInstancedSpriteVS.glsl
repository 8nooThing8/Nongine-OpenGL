#version 460 core

struct ObjectBuffer
{
    mat4 OB_matrix;
};

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 vtxPos;
out vec2 vtxTexCoord;

uniform vec3 pixelPickingID;
out vec3 pixelPickID;

uniform ObjectBuffer objectBuffer;
uniform mat4 invViewMatrix;

void main()
{
    pixelPickID = pixelPickingID;

    mat4 objectMatrix = objectBuffer.OB_matrix;

    vec2 position = aPos;
    vec4 transformedPos = objectMatrix * vec4(position - vec2(0.5f, 0.5f), 0, 1);
    transformedPos.x *= 0.5625f;
    transformedPos = invViewMatrix * transformedPos;

    vec4 ndcPos = transformedPos * 2 - 1;


    vtxPos = position.xy;

    gl_Position = ndcPos;

    vtxTexCoord = aTexCoord;
}