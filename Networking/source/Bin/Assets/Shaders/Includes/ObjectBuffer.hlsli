cbuffer ObjectBuffer : register(b1)
{
    float4 OB_ObjectIDColor;
    
    float4x4 OB_World;
    float4x4 OB_Rotation;
    float4 OB_Scale;
    
    float4 OB_Albedo;
    
    float1 OB_Metalic;
    
    float1 OB_Roughness;
    
    float1 OB_AOStrenght;
    float1 OB_NormalStrength;
    
    float OB_BloomStrength;

	bool OB_IsInstanced;

	float2 trashOB;
}