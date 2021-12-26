#ifndef WORLDGEN_H
#define WORLDGEN_H

float
worldgen_perlin_fbm_2d(int   x,
                       int   z,
                       int   seed,
                       int   octaves,
                       float lac,
                       float gain);

#endif
