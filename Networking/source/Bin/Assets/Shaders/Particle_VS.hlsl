#include "Includes/FrameBuffer.hlsli"
#include "Includes/ObjectBuffer.hlsli"
#include "Includes/Particle.hlsli"

ParticleInput main(ParticleInput vertex)
{
    ParticleInput result = vertex;
    
    float4 vxWorldPos = mul(OB_World, vertex.Position);
    
    float4 vxViewPos = mul(FB_InvView, vxWorldPos);
    result.Position = mul(FB_Projection, vxViewPos);
    
    return result;
}