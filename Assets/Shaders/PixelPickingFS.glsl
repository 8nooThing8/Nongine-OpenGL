#version 460 core

uniform sampler2D diffuseTexture;

layout (location = 0) out vec4 FragColor;

in vec2 vtxTexCoord;
in vec3 pixelPickID;

void main()
{
    // AlbedoTexture
    vec4 textureAlbedo = texture(diffuseTexture, vtxTexCoord);
    
    vec3 color = textureAlbedo.aaa * pixelPickID;

    // Final color
    FragColor = vec4(color, textureAlbedo.a);
}