#version 460 core

out vec4 FragColor;

in vec2 vtxTexCoord;

void main()
{
    vec2 texCoord = vtxTexCoord * 2 - 1;

    if(abs(texCoord).x < 0.925f && abs(texCoord).y < 0.925f)
        discard;

    FragColor = vec4(1, 1, 1, 1);
}