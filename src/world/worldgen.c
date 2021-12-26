#include "../util/util.h"
#include "chunk.h"
#include "math.h"

float
worldgen_perlin_fbm_2d(int   x,
                       int   z,
                       int   seed,
                       int   octaves,
                       float lac,
                       float gain)
{
    float amplitude = 0.66f;
    float frequency = 0.53f;
    float sum       = 0.0f;

    for (int i = 0; i < octaves; i++)
    {
        float plus = noise3((float)x / (32.0f / i) * frequency,
                            (float)z / (32.0f / i) * frequency,
                            seed) +
                     0.5f;

        plus *= amplitude;
        sum += plus;
        amplitude *= gain;
        frequency *= lac;
    }

    return fmax(fmin(sum, 1.0f), 0.0f);
}