#version 460 core

out vec4 FragColor;
in vec3 pixelPickID;

void main()
{
    FragColor = vec4(pixelPickID, 1);
}