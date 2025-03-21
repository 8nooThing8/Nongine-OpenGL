#include "Includes/Sprite.hlsli"
#include "Includes/ObjectBuffer.hlsli"

struct SpriteVertex
{
    float2 Position : SV_POSITION;
    float2 Uv : UV;
};

SpriteVSToPSOutput main(SpriteVertex vertex)
{
    SpriteVSToPSOutput result;
    
    float4x4 worldPos = OB_World;

    worldPos[0].w *= 0.001042f;
    worldPos[1].w *= 0.001042f;
    
    worldPos[0].x -= OB_Scale.x - 0.00001f;
    worldPos[1].y -= OB_Scale.y - 0.00001f;
    worldPos[2].z -= OB_Scale.z - 0.00001f;
    
    worldPos[0].x += OB_Scale.x * 0.0010417f;
    worldPos[1].y += OB_Scale.y * 0.0010417f;
    worldPos[2].z += OB_Scale.z * 0.0010417f;
    
    float4 ndcPosition = mul(worldPos, float4(vertex.Position, 0, 1));
    
    result.Position = float4(ndcPosition.xy, 0, 1);
    result.Uv = vertex.Uv;
    
    return result;
}