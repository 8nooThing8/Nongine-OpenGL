#include "Includes/PBLCalculations.hlsli"

struct HoleMask
{
    float4 rtv : SV_TARGET0;
    float4 maskRTV : SV_TARGET1;
};

HoleMask main(DefaultVSToPSOutput input)
{
    HoleMask holeBufferOut;
    
    holeBufferOut.maskRTV = float4(1, 1, 1, 1);
    
    return holeBufferOut;
}