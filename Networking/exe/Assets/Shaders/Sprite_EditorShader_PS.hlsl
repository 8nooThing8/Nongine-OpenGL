#include "Includes/Sprite.hlsli"
#include "Includes/ColorFunctions.hlsli"

Texture2D spriteTexture : register(t30);
SamplerState pointSampler : register(s3);

float4 main(SpriteVSToPSOutput input) : SV_TARGET
{
    float4 diffuse = spriteTexture.Sample(pointSampler, input.Uv);
    float3 backgroundColor;

    float2 minCoord;
    float2 maxCoord;

    float2 uvInPixels;

    float maxEpsilon;
    float minEpsilon;

    float epsilon;

    bool isOnLeftEdge;
    bool isOnRightEdge;
    bool isOnTopEdge;
    bool isOnBottomEdge;

    bool isOutline;

    maxEpsilon = 10.0f;
    minEpsilon = 0.1f;

    if (SE_SelectedSprite >= 0)
    {
        minCoord = SE_spritesMinMax[SE_SelectedSprite].xy;
        maxCoord = SE_spritesMinMax[SE_SelectedSprite].zw;

        uvInPixels = input.Uv * SE_textureResolution;

        epsilon = maxEpsilon / ((SE_ZoomAmount * 0.001f) + 1.0);
        epsilon = clamp(epsilon, minEpsilon, maxEpsilon);

        isOnLeftEdge = (abs(uvInPixels.x - minCoord.x) <= epsilon) && (uvInPixels.y >= minCoord.y && uvInPixels.y <= maxCoord.y);
        isOnRightEdge = (abs(uvInPixels.x - maxCoord.x) <= epsilon) && (uvInPixels.y >= minCoord.y && uvInPixels.y <= maxCoord.y);
        isOnTopEdge = (abs(uvInPixels.y - minCoord.y) <= epsilon) && (uvInPixels.x >= minCoord.x && uvInPixels.x <= maxCoord.x);
        isOnBottomEdge = (abs(uvInPixels.y - maxCoord.y) <= epsilon) && (uvInPixels.x >= minCoord.x && uvInPixels.x <= maxCoord.x);

        isOutline = isOnLeftEdge || isOnRightEdge || isOnTopEdge || isOnBottomEdge;

        if (isOutline)
        {
            return float4(0.f, 0.f, 1.f, 1.f);
        }
    }
    
    if (SE_HoveredSprite >= 0)
    {
        minCoord = SE_spritesMinMax[SE_HoveredSprite].xy;
        maxCoord = SE_spritesMinMax[SE_HoveredSprite].zw;

        uvInPixels = input.Uv * SE_textureResolution;

        epsilon = maxEpsilon / ((SE_ZoomAmount * 0.001f) + 1.0);
        epsilon = clamp(epsilon, minEpsilon, maxEpsilon);

        isOnLeftEdge = (abs(uvInPixels.x - minCoord.x) <= epsilon) && (uvInPixels.y >= minCoord.y && uvInPixels.y <= maxCoord.y);
        isOnRightEdge = (abs(uvInPixels.x - maxCoord.x) <= epsilon) && (uvInPixels.y >= minCoord.y && uvInPixels.y <= maxCoord.y);
        isOnTopEdge = (abs(uvInPixels.y - minCoord.y) <= epsilon) && (uvInPixels.x >= minCoord.x && uvInPixels.x <= maxCoord.x);
        isOnBottomEdge = (abs(uvInPixels.y - maxCoord.y) <= epsilon) && (uvInPixels.x >= minCoord.x && uvInPixels.x <= maxCoord.x);

        isOutline = isOnLeftEdge || isOnRightEdge || isOnTopEdge || isOnBottomEdge;

        if (isOutline)
        {
            return float4(0.f, 0.85f, 1.f, 1.f);
        }
    }

    for (int i = 0; i < SE_numberOfSprites; i++)
    {
        minCoord = SE_spritesMinMax[i].xy;
        maxCoord = SE_spritesMinMax[i].zw;

        uvInPixels = input.Uv * SE_textureResolution;

        epsilon = maxEpsilon / ((SE_ZoomAmount * 0.001f) + 1.0);
        epsilon = clamp(epsilon, minEpsilon, maxEpsilon);

        isOnLeftEdge = (abs(uvInPixels.x - minCoord.x) <= epsilon) && (uvInPixels.y >= minCoord.y && uvInPixels.y <= maxCoord.y);
        isOnRightEdge = (abs(uvInPixels.x - maxCoord.x) <= epsilon) && (uvInPixels.y >= minCoord.y && uvInPixels.y <= maxCoord.y);
        isOnTopEdge = (abs(uvInPixels.y - minCoord.y) <= epsilon) && (uvInPixels.x >= minCoord.x && uvInPixels.x <= maxCoord.x);
        isOnBottomEdge = (abs(uvInPixels.y - maxCoord.y) <= epsilon) && (uvInPixels.x >= minCoord.x && uvInPixels.x <= maxCoord.x);

        isOutline = isOnLeftEdge || isOnRightEdge || isOnTopEdge || isOnBottomEdge;

        if (isOutline)
        {
            return float4(0.f, 1.f, 0.f, 1.f);
        }
    }
    
    if (diffuse.a < 1)
    {
        if (fmod(floor(input.Uv.x * SE_ZoomAmount * 0.03f) + floor(input.Uv.y * SE_ZoomAmount * 0.03f), 2) == 0)
        {
            backgroundColor = float3(0.4f, 0.4f, 0.4f); // Darker gray for even positions
        }
        else
        {
            backgroundColor = float3(0.6f, 0.6f, 0.6f); // Lighter gray for odd positions
        }

	// Blend the background color with the diffuse color based on the alpha value
        float3 mixedColor = lerp(backgroundColor, diffuse.rgb, diffuse.a);
        return float4(mixedColor, 1.0f);
    }

// If the diffuse alpha is 1, return the diffuse color
    return float4(diffuse.rgb, 1);
}