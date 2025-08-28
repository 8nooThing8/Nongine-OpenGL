#version 460 core

struct ObjectBuffer
{
    mat4 OB_matrixNoScale;
    mat4 OB_matrix;
    vec3 OB_scale;
};

layout (location = 0) in vec4 aPos;
layout (location = 1) in mat4 aVertexColor;
layout (location = 5) in mat4x2 aTexCoord;
layout (location = 7) in vec3 aNormal;
layout (location = 8) in vec3 aTangent;
layout (location = 9) in vec3 aBiNormal;


out vec4 vtxLightPos;
out vec3 vtxPos;
out vec3 vtxWorldPos;
out vec4 vtxColor;
out vec2 vtxTexCoord;
out vec3 vtxNormal;
out mat3 TBNMat;


uniform ObjectBuffer objectBuffer;
uniform mat4 invViewMatrix;
uniform mat4 projectionMatrix;

uniform mat4 lightSpaceMatrix;

//uniform mat4 offsets[125];

mat3 CreateTBNMatrix(vec3 tangent, vec3 biNormal, vec3 normal)
{
    mat3 objMat3 = mat3(objectBuffer.OB_matrix); 

    vec3 T = normalize(objMat3 * tangent);
    vec3 N = normalize(objMat3 * normal);

    T = normalize((T - dot(T, N) * N));
    vec3 B = cross(N, T);

    return mat3(T, B, N);
}

void SetScale(inout mat4 aMat, in vec3 size)
{
    aMat[0][0] = size.x;
    aMat[1][1] = size.y;
    aMat[2][2] = size.z;
}
void main()
{
    mat4 objectMatrix = objectBuffer.OB_matrix;

    vec4 globalPos = objectMatrix * aPos;
    vec4 viewPos = invViewMatrix * globalPos;
    vec4 ndcPos = projectionMatrix * viewPos;

    vtxLightPos = lightSpaceMatrix * globalPos;
    vtxPos = aPos.xyz;
    vtxWorldPos = globalPos.xyz;

    gl_Position = ndcPos;
    
    mat3 TBN = CreateTBNMatrix(aTangent, aBiNormal, aNormal);
    TBNMat = TBN;

    vtxNormal = aNormal;

    vtxColor = aVertexColor[0];
    vtxTexCoord = aTexCoord[0];
}
    //SetScale(objectMatrix, objectBuffer.OB_scale);
    //objectMatrix *= objectBuffer.OB_matrixNoScale;

    //globalPos = offsets[gl_InstanceID] * globalPos;