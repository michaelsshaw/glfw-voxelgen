#ifndef WORLD_H
#define WORLD_H

#include "chunk.h"
#include <cglm/cglm.h>

struct chunkstack
{
    int                x;
    int                y;
    struct chunkstack *last;
};

struct world
{
    struct chunk     **chunks;
    int                num_chunks;
    struct chunkstack *stack;
    size_t             size_chunks;
};

#define WORLD_LOAD_RADIUS 8
#define WATER_LEVEL       62

void
world_init();

struct chunk *
world_chunk_load(struct world *world, int x, int z, int update);

struct chunk *
world_get_chunk(struct world *world, int x, int z);
int
world_get_block(struct world *world, int x, int y, int z);

void
world_chunk_check(struct world *world, int x, int z);

int
world_rebuild_next_chunkmesh(struct world *world);

int
world_chunk_loadnext(struct world *world);

int
world_chunk_rebuildnext(struct world *world);

void
world_chunk_sort(struct world *world);

void
world_reset_all(struct world *world);

#endif // WORLD_H
