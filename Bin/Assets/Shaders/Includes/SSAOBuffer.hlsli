cbuffer ssao : register(b8)
{
    float4 SSAO_Kernel[64];
    int SSAO_KernelSize = 64;
    float3 SSAO_trash;
};