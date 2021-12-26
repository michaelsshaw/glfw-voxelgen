#include "block.h"
#include <assert.h>

struct block BLOCKS[256];

unsigned int
block_create(int id, int t_index, char *name, enum block_type type)
{
    struct block b = { .id               = id,
                       .texture_location = { t_index,
                                             t_index,
                                             t_index,
                                             t_index,
                                             t_index,
                                             t_index },
                       .name             = name,
                       .type             = type };

    BLOCKS[id] = b;

    return id;
}

int
block_get_type(int id)
{
    return BLOCKS[id].type;
}

unsigned int
block_createuv(int             id,
               char           *name,
               enum block_type type,
               int             north,
               int             east,
               int             south,
               int             west,
               int             up,
               int             down)
{
    struct block b = {
        .id               = id,
        .texture_location = { north, east, south, west, up, down },
        .name             = name,
        .type             = type
    };

    BLOCKS[id] = b;

    return id;
}

void
block_get_texcoord(int block, float *x, float *y, int direction)
{
    int tx = BLOCKS[block].texture_location[direction] % 16;
    int ty = BLOCKS[block].texture_location[direction] / 16;

    *x = (float)tx / 16.0f;
    *y = (float)-ty / 16.0f + 1.0f;
}

void
block_init()
{
    block_create(BLOCK_AIR, 0, "Air", BLOCK_INVISIBLE);
    block_createuv(BLOCK_GRASS, "Grass", BLOCK_SOLID, 4, 4, 4, 4, 0, 5);
    block_create(BLOCK_DIRT, 5, "Dirt", BLOCK_SOLID);
    block_create(BLOCK_PLANK, 1, "Wood plank", BLOCK_SOLID);
    block_create(BLOCK_STONE, 2, "Stone", BLOCK_SOLID);
    block_create(BLOCK_BEDROCK, 3, "Bedrock", BLOCK_SOLID);
    block_create(BLOCK_WATER, 6, "Water", BLOCK_TRANSPARENT);
    block_create(BLOCK_SAND, 7, "Sand", BLOCK_SOLID);
}