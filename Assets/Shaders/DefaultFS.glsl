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

uniform sampler2D shadowMap;

struct Light
{
    float intensity;
    vec3 lightColor;
    vec3 lightDir;
};


out vec4 FragColor;

in vec4 vtxLightPos;
in vec3 vtxPos;
in vec3 vtxWorldPos;
in vec4 vtxColor;
in vec2 vtxTexCoord;
in vec3 vtxNormal;
in mat3 TBNMat;

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

float ShadowCalculation(vec3 aNormal)
{
    vec3 projectedCoord = vtxLightPos.xyz / vtxLightPos.w;
    projectedCoord = projectedCoord * 0.5f + 0.5f;

    if(projectedCoord.z > 1.0)
        return 1.f;

    float bias = 0.0000005f;  
    float currentDepth = projectedCoord.z;
    
    float shadow = 0.0;
    vec2 texelSize = 1.2f / textureSize(shadowMap, 0);
    int kernalSize = 2;
    int samples = 0;
    for(int x = -kernalSize; x <= kernalSize; ++x)
    {
        for(int y = -kernalSize; y <= kernalSize; ++y)
        {
            float pcfDepth = texture(shadowMap, projectedCoord.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth > pcfDepth ? 0.0f : 1.0f;        
            samples++;
        }    
    }
    shadow /= samples;

    return shadow;
}

void main()
{
    if(material.albedo.a <= 0.05f)
        discard;


    // AlbedoTexture
    vec4 textureAlbedo = texture(material.diffuseTexture, vtxTexCoord);

    // Normal
    vec3 normal = texture(material.normalTexture, vtxTexCoord).rgb;
    normal = normal * 2.f - 1.f;
    normal = normalize(TBNMat * normal);

    // Shadows
    float shadow = ShadowCalculation(normal);

    // Diffuse
    float diff = max(dot(normal, -light.lightDir), 0.0);
    vec3 diffuse = diff * light.lightColor * textureAlbedo.rgb * shadow;

    // Specular
    vec3 viewDir = normalize(viewPos - vtxWorldPos);
    vec3 reflectDir = reflect(light.lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    float specular = material.specularStrength * spec;  

    vec3 ambient = vec3(0.15f, 0.15f, 0.15f) * textureAlbedo.rgb;


    // Final color
    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result * material.albedo.rgb, material.albedo.a * textureAlbedo.a);
}