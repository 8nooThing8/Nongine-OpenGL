struct ParticleVertexBuffer
{
	float4 color;
	float3 position;
    float size;
};


cbuffer particleBuffer : register(b9)
{
    ParticleVertexBuffer particles[4096];

	float3 rotation;

	unsigned int particleCount;
};