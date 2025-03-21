struct ParticleInput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float4 ChannelMask : CHANNELMASK;
    float3 Velocity : VELOCITY;
    float Lifetime : LIFETIME;
    float Angle : ANGLE;
    float2 Size : Size;
};

struct ParticleGStoPS
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 UV : UV;
    float Lifetime : LIFETIME;
};

struct TrailInput
{
    float4 Position : SV_POSITION;
    
    float2 Uv : UV;
};