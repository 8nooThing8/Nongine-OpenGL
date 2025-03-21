#include "Includes/Particle.hlsli"
#include "Includes/FrameBuffer.hlsli"


[maxvertexcount(4)]
void main(point ParticleInput input[1], inout TriangleStream<ParticleGStoPS> output)
{
    const float2 offsets[4] =
    {
        { -1.0f, 1.0f },
        { 1.0f, 1.0f },
        { -1.0f, -1.0f },
        { 1.0f, -1.0f }
    };
    const float2 uvs[4] =
    {
        { 0.0f, 0.0f },
        { 1.0f, 0.0f },
        { 0.0f, 1.0f },
        { 1.0f, 1.0f }
    };
    
    const ParticleInput vertex = input[0];
    
    const float PI = 3.14159265358979323846f;
    const float DEGTORAD = PI / 180.0f;
    
    for (uint i = 0; i < 4; ++i)
    {
        ParticleGStoPS result;
        
        result.Lifetime = vertex.Lifetime;
        result.Color = vertex.Color;
        
        result.Position = vertex.Position;
        float4 origin = result.Position;
        
        float rad = DEGTORAD * vertex.Angle;
        
        // Rotate result.Position around Origin.
        // I use Degrees so vertex.Angle is Deg.
        // Hence DEGTORAD above.
        
        float sizeY = vertex.Size.y * FB_AspectRatio;
        
        result.Position.xy = float2
        (
            origin.x + offsets[i].x * vertex.Size.x * cos(rad) - offsets[i].y * sizeY * sin(rad),
            origin.y + offsets[i].y * sizeY * cos(rad) + offsets[i].x * vertex.Size.x * sin(rad)
        );
        
        result.UV = uvs[i];
        // Transfer Color, Lifetime, Velocity
        // and ChannelMask from Vertex.
        // Done
        output.Append(result);
    }
}