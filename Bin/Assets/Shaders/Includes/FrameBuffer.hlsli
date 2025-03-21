cbuffer FrameBuffer : register(b0)
{
    float4x4 FB_InvView;
    float4x4 FB_Projection;
    float4 FB_CamRot;
    
    float4 FB_CameraPos;
    
    float3 FB_EyeDir;

    float FB_AspectRatio;
}