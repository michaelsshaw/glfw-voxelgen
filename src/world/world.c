#include "world.h"
#include "../state.h"
#include "block.h"
#include <math.h>
#include <pthread.h>
#include <stdlib.h>

int
world_chunk_loaded(struct world *world, int x, int z)
{
    for (int i = 0; i < world->num_chunks; i++)
    {
        if (world->chunks[i]->x == x && world->chunks[i]->z == z)
        {
            return 1;
        }
    }

    return 0;
}

struct chunk *
world_chunk_load(struct world *world, int x, int z, int update)
{
    if (world_chunk_loaded(world, x, z))
    {
        return NULL;
    }
    struct chunk *chunk = chunk_init(x, z, CHUNKTYPE_NORMAL);
    chunk->built = 0;
    if (world->size_chunks < (world->num_chunks + 1) * sizeof(struct chunk))
    {
        size_t size_new    = (world->num_chunks * 2) * sizeof(struct chunk);
        world->chunks      = realloc(world->chunks, size_new);
        world->size_chunks = size_new;
    }

    for (int xo = -1; xo <= 1; xo++)
    {
        for (int zo = -1; zo <= 1; zo++)
        {
            if (xo == 0 && zo == 0)
                continue;

            struct chunk *neighbor = world_get_chunk(world, x + xo, z + zo);
            if (neighbor == NULL)
                continue;

            neighbor->lightstatus = CSTATUS_OUTDATED;
        }
    }

    world->chunks[world->num_chunks] = chunk;
    world->num_chunks++;

    chunk->dist = chunk_dist(x, z);

    world_chunk_sort(world);

    return chunk;
}

static struct chunk *
world_binsearch(struct world *world, int low, int high, int x, int z)
{
    if (high < low)
        return NULL;

    int           mid = (low + high) / 2;
    struct chunk *c   = world->chunks[mid];

    float dist = chunk_dist(x, z);

    if (c->x == x && c->z == z)
        return c;
    if (c->dist < dist)
        return world_binsearch(world, mid + 1, high, x, z);
    else
        return world_binsearch(world, low, mid - 1, x, z);
}

struct chunk *
world_get_chunk(struct world *world, int x, int z)
{
    // use binsearch first
    struct chunk *out = world_binsearch(world, 0, world->num_chunks - 1, x, z);

    if (out != NULL)
    {
        return out;
    }
    // if failed, bruteforce it (because sometimes we have sorting errors)
    for (int i = 0; i < world->num_chunks; i++)
    {
        if (world->chunks[i]->x == x && world->chunks[i]->z == z)
        {
            return world->chunks[i];
        }
    }

    return NULL;
}
/*
 * world_chunk_push
 *
 * add chunk coordinates to the chunkstack
 */

void
world_chunk_push(struct world *world, int x, int y)
{
    struct chunkstack *n_node = malloc(sizeof(struct chunkstack));

    n_node->x    = x;
    n_node->y    = y;
    n_node->last = NULL;

    if (world->stack != NULL)
    {
        n_node->last = world->stack;
    }

    world->stack = n_node;
}

/*
 * world_chunk_pop
 *
 * returns 0 if the queue is empty, 1 if a value is returned
 */

int
world_chunk_pop(struct world *world, int *x, int *y)
{
    if (world->stack == NULL)
    {
        return 0;
    }

    struct chunkstack *tmp = world->stack;

    *x           = tmp->x;
    *y           = tmp->y;
    world->stack = tmp->last;

    free(tmp);

    return 1;
}

int
world_chunk_loadnext(struct world *world)
{
    int x, y;

    int result = world_chunk_pop(world, &x, &y);

    if (result == 0)
    {
        return 0;
    }

    world_chunk_load(world, x, y, 1);

    return 1;
}

int
world_chunk_rebuildnext(struct world *world)
{
    for (int i = world->num_chunks - 1; i >= 0; i--)
    {
        struct chunk *chunk = world->chunks[i];
        if (chunk->lightstatus != CSTATUS_READY)
        {
            chunk_build_light(chunk);
            break;
        }

        if (chunk->mesh->status != CSTATUS_READY)
        {
            chunkmesh_build(chunk, chunk->mesh, BLOCK_SOLID);
            chunkmesh_build(chunk, chunk->mesh_trans, BLOCK_TRANSPARENT);
            break;
        }
    }

    return 0;
}

void
world_chunk_unload(struct world *world, int i)
{
    // free the chunk and chunkmesh memory
    chunk_destroy(world->chunks[i]);

    // remove the freed chunk from the world

    for (int j = i + 1; j < world->num_chunks; j++)
    {
        world->chunks[j - 1] = world->chunks[j];
    }

    world->num_chunks--;
}

int
world_chunk_cmpfunc(const void *aa, const void *bb)
{
    struct chunk *a = *(struct chunk **)aa;
    struct chunk *b = *(struct chunk **)bb;
    return a->dist < b->dist;
}

void
world_chunk_sort(struct world *world)
{
    qsort(world->chunks,
          world->num_chunks,
          sizeof(struct chunk **),
          world_chunk_cmpfunc);
}

/*
 * world_chunk_check
 *
 * check chunks around the player in the world_radius
 * enqueue chunks that have not been loaded but should
 */

void
world_chunk_check(struct world *world, int x, int z)
{
    int num_chunks = world->num_chunks;

    // remove chunks when too many are loaded
    if (num_chunks > pow(STATE.world_radius * 2 + 4, 2))
    {
        int i = 0;
        while (i < world->num_chunks)
        {
            struct chunk *chunk = world->chunks[i];

            if (abs(chunk->x - x) > STATE.world_radius ||
                abs(chunk->z - z) > STATE.world_radius)
            {
                world_chunk_unload(world, i);
            }
            else
            {
                i++;
            }
        }
    }

    // Buffer an area around the player to provide seamless transition
    for (int xx = x - STATE.world_radius; xx < x + STATE.world_radius; xx++)
    {
        for (int zz = z - STATE.world_radius; zz < z + STATE.world_radius; zz++)
        {
            if (!world_chunk_loaded(world, xx, zz))
            {
                world_chunk_push(world, xx, zz);
            }
        }
    }

    // sort chunks for transparency
    for (int i = 0; i < world->num_chunks; i++)
    {
        struct chunk *c = world->chunks[i];

        c->dist = chunk_dist(c->x, c->z);
    }

    world_chunk_sort(world);
}

void
world_reset_all(struct world *world)
{
    for (int i = 0; i < world->num_chunks; i++)
    {
        struct chunk *c = world->chunks[i];
        c->lightstatus  = CSTATUS_OUTDATED;

        if (c->mesh && c->mesh->status != CSTATUS_EMPTY)
        {
            c->mesh->status = CSTATUS_OUTDATED;
        }
    }
}

void
world_init()
{
    STATE.world_radius = 12;
    world_chunk_check(&STATE.world, 0, 0);
    int x = STATE.cam_main.position[0];
    int z = STATE.cam_main.position[2];
}
