#include <iostream>
#include <UtilityFunctions.hpp>

#include <Vector/Vector.hpp>

namespace CommonUtilities
{
    const Vector2<float> RandomGradient(int aIX, int aIY)
    {
        const unsigned w = 8 * sizeof(unsigned);
        const unsigned s = static_cast<unsigned>(w * 0.5f);

        unsigned a = aIX, b = aIY;
        a *= 3284157443;
        b ^= a << s | a >> (w - s);
        b *= 1911520717;
        a ^= b << s | b >> (w - s);
        a *= 2048419325;

        const float random = static_cast<float>(a * (3.14159265 / ~(~0u >> 1))); // in [0, 2*Pi]
        
        const Vector2<float> v(cos(random), sin(random));

        return v;
    }

    float DotGridGradient(int aIX, int aIY, float x, float y)
    {
        Vector2<float> gradient = RandomGradient(aIX, aIY);

        const Vector2<float> distance(x - static_cast<float>(aIX), y - static_cast<float>(aIY));

        return distance.Dot(gradient);
    }

    float Perlin(float x, float y)
    {
        // Determine grid cell coordinates
        int x0 = (int)floor(x);
        int x1 = x0 + 1;
        int y0 = (int)floor(y);
        int y1 = y0 + 1;

        // Determine interpolation weights
        // Could also use higher order polynomial/s-curve here
        float sx = x - (float)x0;
        float sy = y - (float)y0;

        // Interpolate between grid point gradients
        float n0, n1, ix0, ix1, value;

        n0 = DotGridGradient(x0, y0, x, y);
        n1 = DotGridGradient(x1, y0, x, y);
        ix0 = CubicInterporlation(n0, n1, sx);

        n0 = DotGridGradient(x0, y1, x, y);
        n1 = DotGridGradient(x1, y1, x, y);
        ix1 = CubicInterporlation(n0, n1, sx);

        value = CubicInterporlation(ix0, ix1, sy);
        return value;
    }

    static float perlinNoisePos(int x, int y, int gridSize, int aSeed)
    {
        float val = 0.f;
        
        float freq = 1.f;
        float amp = 1.f;
        
        for (int k = 0; k < 12; k++)
        {
            val += Perlin(static_cast<float>(static_cast<float>(y + aSeed * 1523) * freq / static_cast<float>(gridSize)),
                                       static_cast<float>(static_cast<float>(x + aSeed * 1523) * freq / static_cast<float>(gridSize))) * amp;

            freq *= 2.f;
            amp *= 0.5f;
        }

        val *= 1.2f;

        if (val > 1.f)
            val = 1.f;
        else if (val < -1.f)
            val = -1.f;

        return val;
    }

    float CubicInterporlation(float aMin, float aMax, float aValue)
    {
        return static_cast<float>((aMax - aMin) * (3.f - aValue * 2.f) * aValue * aValue + aMin);
    }

    static std::vector<std::vector<float>> GeneratePerinMap(CommonUtilities::Vector2<int> aTextureSize,
                                                                  int gridSize, int aSeed)
    {
        std::vector<std::vector<float>> textureMap;
        std::vector<float> textureMap1D;


        for (int x = 0; x < aTextureSize.x; ++x)
        {
            textureMap1D.clear();
            
            for (int y = 0; y < aTextureSize.y; ++y)
            {
                float val = perlinNoisePos(x, y, gridSize, aSeed);

                textureMap1D.push_back(val);
            }

            textureMap.push_back(textureMap1D);
        }

        return textureMap;
    }
}
