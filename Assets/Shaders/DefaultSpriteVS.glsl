#version 460 core

// struct ObjectBuffer
// {
//     mat4 OB_matrix;
// };

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in mat4 instanceMat;
layout (location = 6) in vec4 pixelPickInstance;

out vec2 vtxPos;
out vec2 vtxTexCoord;

//uniform vec3 pixelPickingID;
out vec3 pixelPickID;

//uniform ObjectBuffer objectBuffer;
uniform mat4 invViewMatrix;

//uniform mat4 matricies[200];
//uniform int instanceCount;

void main()
{
    pixelPickID = pixelPickInstance.rgb;

    //mat4 objectMatrix = aInstanceMat;

    vec4 transformedPos = instanceMat * vec4(aPos - vec2(0.5f, 0.5f), 0, 1);
    transformedPos.x *= 0.5625f;
    transformedPos = invViewMatrix * transformedPos;

    vec4 ndcPos = transformedPos * 2 - 1;

ndcPos.z = 0;

    vtxPos = aPos;

    gl_Position = ndcPos;

    vtxTexCoord = aTexCoord;
}