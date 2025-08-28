#version 460 core

uniform sampler2D pixelPickTexture;

out vec4 FragColor;

in vec2 vtxTexCoord;

void main()
{
    mediump vec4 total = vec4(0.0);
    float kernelSize = 4.0;
    float weightSum = 0.0;

    for (float x = -kernelSize; x <= kernelSize; x += 1.0) {
        for (float y = -kernelSize; y <= kernelSize; y += 1.0) 
        {
            float weight = 1.0 / (1.0 + abs(x) + abs(y));
            weightSum += weight;
        
            vec2 offset = vec2(x / 1920.0, y / 1080.0);

            if(texture2D(pixelPickTexture, vtxTexCoord + offset).a > 0)
                total += vec4(1, 1, 1, 1) * weight;
        }
    }

    FragColor = vec4(1, 0.5, 0, total / weightSum);
}