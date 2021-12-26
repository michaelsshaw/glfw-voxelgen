#ifndef CHUNK_H
#define CHUNK_H

#include <cglm/cglm.h>

#define CHUNK_SIZE   16
#define CHUNK_HEIGHT 128
#define CHUNK_VOLUME 32768

#define chunk_foreach()                                                        \
    for (int z = 0; z < CHUNK_SIZE; z++)                                       \
        for (int y = 0; y < CHUNK_HEIGHT; y++)                                 \
            for (int x = 0; x < CHUNK_SIZE; x++)

#define chunk_xzforeach()                                                      \
    for (int z = 0; z < CHUNK_SIZE; z++)                                       \
        for (int x = 0; x < CHUNK_SIZE; x++)

enum chunktype
{
    CHUNKTYPE_NORMAL,
    CHUNKTYPE_EMPTY
};

enum DIRECTIONS
{
    NORTH,
    EAST,
    SOUTH,
    WEST,
    UP,
    DOWN
};

enum CHUNKMESH_STATUS
{
    CSTATUS_EMPTY,
    CSTATUS_BUILD,
    CSTATUS_OUTDATED,
    CSTATUS_READY
};

struct chunkmesh
{
    int vertex_count;
    int index_count;
    int triangle_count;

    // enum CHUNKMESH_STATUS
    short status;

    // OpenGL integer identifiers for the VAO, VBO and EBO
    // Only used in draw calls
    unsigned int VAO, VBO, EBO;
};

struct light
{
    int           level;
    struct chunk *chunk;
    int           x, y, z;
};

struct lightqueue
{
    struct light *head;
    struct light *tail;
};

struct chunk
{

    int  x, z; // CHUNK STATE.world POSITION (int)
    int *data;
    int *lightdata;

    short lightstatus;

    short built;

    struct light **lights;
    int            num_lights;
    int            max_lights;

    struct chunkmesh *mesh;
    struct chunkmesh *mesh_trans;

    float dist;
};

int
chunk_get_block(struct chunk *chunk, int x, int y, int z);

void
chunk_column_fill(struct chunk *chunk, int x, int y, int z);

void
chunkmesh_build(struct chunk *chunk, struct chunkmesh *mesh, int type);

struct chunk *
chunk_init(int x, int z, enum chunktype chunktype);

void
chunk_destroy(struct chunk *chunk);

void
chunkmesh_destroy(struct chunkmesh *mesh);

float
chunk_dist(int x, int z);

void
chunk_build_light(struct chunk *chunk);

#endif
