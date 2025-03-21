#version 460 core

struct Material
{
    sampler2D diffuse;
    sampler2D materialTex;
    float specularStrength;
    float shininess;
};

struct Light
{
    vec3 lightPos;
    vec3 lightColor;
    vec3 lightDir;
};

out vec4 FragColor;

in vec3 vtxPos;
in vec3 vtxWorldPos;

in vec4 vtxColor;
in vec2 texCoord;
in vec3 normal;

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main()
{
    // albedo
    vec3 textureAlbedo = texture(material.diffuse, texCoord).rgb;

    if(gl_FrontFacing)
    {
        textureAlbedo = texture(material.materialTex, texCoord).rgb;
    }

    // Diffuse
    //vec3 lightDir = normalize(light.lightPos - vtxWorldPos);  
    float diff = max(dot(normal, light.lightDir), 0.0);
    vec3 diffuse = diff * light.lightColor * textureAlbedo;

    // Specular
    vec3 viewDir = normalize(viewPos - vtxWorldPos);
    vec3 reflectDir = reflect(-light.lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    float specular = material.specularStrength * spec;  

    vec3 ambient = vec3(0.2f, 0.2f, 0.2f) * textureAlbedo;

    // Final color
    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1);
}