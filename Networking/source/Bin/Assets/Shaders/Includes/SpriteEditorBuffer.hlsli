#define MaxSprites 256

cbuffer SpriteEditorBuffer : register(b7)
{
    float4 SE_spritesMinMax[MaxSprites];

    float2 SE_textureResolution;

    int SE_numberOfSprites;
    
    float SE_ZoomAmount;
    
    int SE_SelectedSprite;
    int SE_HoveredSprite;
    
    float2 SE_trash;
}