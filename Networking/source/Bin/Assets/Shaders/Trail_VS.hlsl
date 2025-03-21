#include "Includes/FrameBuffer.hlsli"
#include "Includes/ObjectBuffer.hlsli"
#include "Includes/Particle.hlsli"

TrailInput main(TrailInput vertex)
{
    TrailInput result;

    float4 vxWorldPos = mul(OB_World, vertex.Position);
    float4 vxViewPos = mul(FB_InvView, vxWorldPos);
    float4 ndcCoords = mul(FB_Projection, vxViewPos);
    
    result.Position = ndcCoords;

    result.Uv = vertex.Uv;
    
    return result;
}