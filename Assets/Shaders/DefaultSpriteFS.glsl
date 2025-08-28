#version 460 core

struct Material
{
    sampler2D diffuseTexture;
    sampler2D normalTexture;
    sampler2D ambientOcculusionTexture;
    sampler2D roughnessTexture;
    sampler2D metallicTexture;

    vec4 albedo;
    float specularStrength;
    float shininess;
};

out vec4 FragColor;

in vec2 vtxPos;
in vec2 vtxTexCoord;

uniform Material material;

void main()
{
    // AlbedoTexture
    vec4 textureAlbedo = texture(material.diffuseTexture, vtxTexCoord);

    float alpha = material.albedo.a * textureAlbedo.a;
    if(alpha <= 0.05f)
        discard;

    // Normal
    //vec3 normal = texture(material.normalTexture, vtxTexCoord).rgb;
    //normal = normal * 2.f - 1.f;

    // Diffuse
    //float diff = max(dot(normal, -light.lightDir), 0.0);
    //vec3 diffuse = diff * light.lightColor * textureAlbedo.rgb * shadow;

    vec3 diffuse = textureAlbedo.rgb;
    
    // Final color
    FragColor = vec4(diffuse, alpha);
}