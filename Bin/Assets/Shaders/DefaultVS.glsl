#version 460 core

layout (location = 0) in vec4 aPos;
layout (location = 1) in mat4 aVertexColor;
layout (location = 5) in mat4x2 aTexCoord;
layout (location = 7) in vec3 aNormal;

out vec3 vtxPos;
out vec3 vtxWorldPos;

out vec4 vtxColor;
out vec2 texCoord;
out vec3 normal;

uniform mat4 objectMatrix;
uniform mat4 invViewMatrix;
uniform mat4 projectionMatrix;

//uniform mat4 offsets[125];

void main()
{
    vec4 globalPos = objectMatrix * aPos;
    //globalPos = offsets[gl_InstanceID] * globalPos;
    vec4 viewPos = invViewMatrix * globalPos;
    vec4 ndcPos = projectionMatrix * viewPos;

    vtxPos = aPos.xyz;
    vtxWorldPos = globalPos.xyz;

    gl_Position = ndcPos;

    normal = transpose(inverse(mat3(objectMatrix))) * aNormal;  

//    vtxColor = aVertexColor;
    texCoord = aTexCoord[0];
}