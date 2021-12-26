#include <assert.h>
#include <glad/glad.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../util/util.h"
#include "block.h"
#include "chunk.h"
#include "worldgen.h"

#include "../state.h"

#define VERTEX_STRIDE 7

#define SQUARE(A) ((A) * (A))

#define FOR3(A)                                                                \
    typeof(A) _a = (A);                                                        \
    for (int _z = 0; _z < _a; _z++)                                            \
        for (int _y = 0; _y < _a; _y++)                                        \
            for (int _x = 0; _x < _a; _x++)

static int FACE_INFO[] = {
    0, 4, 7, 3, 5, // north (+x) 0
    4, 5, 6, 7, 5, // east  (+z) 1
    5, 1, 2, 6, 5, // south (-x) 2
    0, 1, 2, 3, 5, // west  (-z) 3
    4, 0, 1, 5, 4, // up    (+y) 4
    2, 6, 7, 3, 30 // down  (-y) 5
};

static unsigned int FACE_INDICES[] = { 0, 1, 2, 0, 2, 3 };

static int FACE_VERTICES[] = {
    1, 1, 0, //
    0, 1, 0, //
    0, 0, 0, //
    1, 0, 0, //

    1, 1, 1, //
    0, 1, 1, //
    0, 0, 1, //
    1, 0, 1, //
};

static float FACE_UVS[] = {
    1, 0, //
    0, 0, //
    0, 1, //
    1, 1  //
};

static vec3 DIR_VECTORS[] = {
    { 1, 0, 0 },  //
    { 0, 0, 1 },  //
    { -1, 0, 0 }, //
    { 0, 0, -1 }, //
    { 0, 1, 0 },  //
    { 0, -1, 0 },
};

/*
 * chunk_get_block
 *
 * return blockid of block at given chunk coordinates
 */

int
chunk_get_block(struct chunk *chunk, int x, int y, int z)
{
    if (x >= CHUNK_SIZE || y >= CHUNK_HEIGHT || z >= CHUNK_SIZE || x < 0 ||
        y < 0 || z < 0 || !chunk || !chunk->data)
    {
        return 0;
    }

    int block = chunk->data[x * CHUNK_SIZE * CHUNK_HEIGHT + y * CHUNK_SIZE + z];
    if (!block)
        block = BLOCK_AIR;

    return block;
}

/*
 * chunk_set_block
 *
 * set blockid of block at given chunk coordinates
 */

void
chunk_set_block(struct chunk *chunk, int blocktype, int x, int y, int z)
{
    assert(blocktype > -1);
    assert(x > -1 && y > -1 && z > -1);
    assert(x < CHUNK_SIZE && y < CHUNK_HEIGHT && z < CHUNK_SIZE);

    chunk->data[x * CHUNK_SIZE * CHUNK_HEIGHT + y * CHUNK_SIZE + z] = blocktype;
}

/*
 * chunk_set_torchlight
 *
 * set light of block at given chunk coordinates
 */

void
chunk_set_torchlight(struct chunk *chunk, int level, int x, int y, int z)
{
    assert(level >= 0);
    assert(x > -1 && y > -1 && z > -1);
    assert(x < CHUNK_SIZE && y < CHUNK_HEIGHT && z < CHUNK_SIZE);

    struct light *light = malloc(sizeof(struct light));
    light->level        = level;
    light->chunk        = chunk;

    light->x = x;
    light->y = y;
    light->z = z;

    if (chunk->num_lights == chunk->max_lights)
    {
        chunk->max_lights += 1.5;
        chunk->lights =
          realloc(chunk->lights, chunk->max_lights * sizeof(struct light));
    }

    chunk->lights[chunk->num_lights] = light;
    chunk->num_lights++;
}

/*
 * chunk_get_torchlight
 *
 * return blockid of block at given chunk coordinates
 */

int
chunk_get_torchlight(struct chunk *chunk, int x, int y, int z)
{
    if (x >= CHUNK_SIZE || y >= CHUNK_HEIGHT || z >= CHUNK_SIZE || x < 0 ||
        y < 0 || z < 0 || !chunk || !chunk->lightdata)
    {
        return 0;
    }

    int level =
      chunk->lightdata[x * CHUNK_SIZE * CHUNK_HEIGHT + y * CHUNK_SIZE + z];

    return level;
}

void
chunk_build_light(struct chunk *chunk)
{
    for (int i = 0; i < chunk->num_lights; i++)
    {
        struct light *light = chunk->lights[i];
        int           x     = light->x;
        int           y     = light->y;
        int           z     = light->z;

        FOR3(light->level * 2)
        {
            // offset coordinates from center of light
            int xo = _x - light->level;
            int yo = _y - light->level;
            int zo = _z - light->level;

            if (abs(xo) > light->level || abs(yo) > light->level ||
                abs(zo) > light->level)
            {
                continue;
            }
            // new chunk-coordinates
            int nx = x + xo;
            int ny = y + yo;
            int nz = z + zo;

            struct chunk *c = chunk;
            // bounds checking
            if (nx < 0 || nz < 0 || nx >= CHUNK_SIZE || nz >= CHUNK_SIZE)
            {
                int checkx = (nx < 0) ? nx - CHUNK_SIZE : nx;
                int checkz = (nz < 0) ? nz - CHUNK_SIZE : nz;

                c = world_get_chunk(&STATE.world,
                                    chunk->x + checkx / CHUNK_SIZE,
                                    chunk->z + checkz / CHUNK_SIZE);

                nx %= CHUNK_SIZE;
                nz %= CHUNK_SIZE;
                if (c == NULL)
                {
                    continue;
                }
            }

            int neighbor = chunk_get_block(c, nx, ny, nz);
            int ntype    = block_get_type(neighbor);
            int nlight   = chunk_get_torchlight(c, nx, ny, nz);

            if ((ntype == BLOCK_TRANSPARENT || ntype == BLOCK_INVISIBLE) &&
                nlight + 2 < light->level)
            {
                c->lightdata[nx * CHUNK_SIZE * CHUNK_HEIGHT + ny * CHUNK_SIZE +
                             nz] =
                  fmax(0, light->level - (abs(xo) + abs(yo) + abs(zo)));
                if (c->mesh->status != CSTATUS_EMPTY)
                {
                    c->mesh->status = CSTATUS_OUTDATED;
                }
            }
        }
    }

    chunk->lightstatus = CSTATUS_READY;
}

/*
 * chunk_column_fill
 *
 * fill a column to a certain height given blocktype constraints
 */

void
chunk_column_fill(struct chunk *chunk, int x, int y, int z)
{
    int blocktype;
    for (int i = 0; i <= y; i++)
    {
        if (i == 0)
            blocktype = BLOCK_BEDROCK;
        else if (i < WATER_LEVEL)
            blocktype = BLOCK_STONE;
        else if (i - WATER_LEVEL < 2)
            blocktype = BLOCK_SAND;
        else if (i == y)
            blocktype = BLOCK_GRASS;
        else if (y - i < 2)
            blocktype = BLOCK_DIRT;
        else if (i > y)
            blocktype = BLOCK_AIR;
        else
            blocktype = BLOCK_STONE;

        chunk_set_block(chunk, blocktype, x, i, z);

        if (blocktype == BLOCK_GRASS && x == 7 && z == 0)
        {
            chunk_set_torchlight(chunk, 7, x, i + 1, z);
        }
    }
}

/*
 * face_emit
 *
 * emit to the chunkmesh a face of a block
 * in a given direction
 */

static void
face_emit(struct chunkmesh *mesh,
          float            *vertices,
          int              *indices,
          int               block,
          int               direction,
          int               x,
          int               y,
          int               z,
          int               lightlevel)
{
    float tx, ty;
    block_get_texcoord(block, &tx, &ty, direction);

    // emit vertices and texcoords
    for (int i = 0; i < 4; i++)
    {
        int ind_fv = FACE_INFO[direction * 5 + i] * 3;
        int ind_cm = mesh->vertex_count + i * VERTEX_STRIDE;
        // append vertices
        vertices[ind_cm++] = x + FACE_VERTICES[ind_fv];
        vertices[ind_cm++] = y + FACE_VERTICES[ind_fv + 1];
        vertices[ind_cm++] = z + FACE_VERTICES[ind_fv + 2];

        // append texcoord as a vec2 clamped between
        // the two values representing the 16x16 texture
        // y is flipped since our tex atlas  (0, 0) is
        // at the top left, but in opengl (0, 0) is bottom left
        vertices[ind_cm++] = FACE_UVS[i * 2] * TEXCOORD_STEP + tx;
        vertices[ind_cm++] = FACE_UVS[i * 2 + 1] * -TEXCOORD_STEP + ty;

        // TODO: remove this
        // lighting divisor for each face
        vertices[ind_cm++] = FACE_INFO[direction * 5 + 4];

        // light level for this face
        vertices[ind_cm++] = lightlevel;
    }

    // emit 6 indices
    for (int i = 0; i < 6; i++)
    {
        indices[mesh->index_count] =
          mesh->vertex_count / VERTEX_STRIDE + FACE_INDICES[i];

        mesh->index_count++;
    }

    mesh->vertex_count += VERTEX_STRIDE * 4;
    mesh->triangle_count += 2;
}

/*
 * chunkmesh_build
 *
 * build a chunk mesh using the block data
 * and buffer the data into a VAO, VBO and EBO
 */

void
chunkmesh_build(struct chunk *chunk, struct chunkmesh *mesh, int type)
{
    int    vol      = CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE;
    float *vertices = calloc(CHUNK_VOLUME * VERTEX_STRIDE * 24, sizeof(float));
    int   *indices  = calloc(CHUNK_VOLUME * 36, sizeof(int));
    mesh->vertex_count   = 0;
    mesh->index_count    = 0;
    mesh->triangle_count = 0;

    chunk_foreach()
    {
        int block = chunk_get_block(chunk, x, y, z);
        if (block == BLOCK_AIR || block_get_type(block) != type)
        {
            continue;
        }
        for (int i = 0; i < 6; i++)
        {

            int neighbor = chunk_get_block(chunk,
                                           x + DIR_VECTORS[i][0],
                                           y + DIR_VECTORS[i][1],
                                           z + DIR_VECTORS[i][2]);

            int type_neighbor = block_get_type(neighbor);

            int nlight = chunk_get_torchlight(chunk,
                                              x + DIR_VECTORS[i][0],
                                              y + DIR_VECTORS[i][1],
                                              z + DIR_VECTORS[i][2]);

            if (type_neighbor == type)
            {
                continue;
            }

            if (type == BLOCK_TRANSPARENT && i != UP)
            {
                continue;
            }

            face_emit(mesh,
                      vertices,
                      indices,
                      block,
                      i,
                      x - chunk->x + chunk->x * CHUNK_SIZE,
                      y,
                      z - chunk->z + chunk->z * CHUNK_SIZE,
                      nlight);
        }
    }

    unsigned int VAO, VBO, EBO;
    // either reuse existing VAO or generate new ones
    if (mesh->status == CSTATUS_EMPTY)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        mesh->VAO = VAO;
        mesh->VBO = VBO;
        mesh->EBO = EBO;
    }
    else
    {
        VAO = mesh->VAO;
        VBO = mesh->VBO;
        EBO = mesh->EBO;
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * mesh->vertex_count,
                 vertices,
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(int) * mesh->index_count,
                 indices,
                 GL_STATIC_DRAW);
    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
      0, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE * sizeof(float), (void *)0);
    // texcoords

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          VERTEX_STRIDE * sizeof(float),
                          (void *)(3 * sizeof(float)));
    // light value
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,
                          1,
                          GL_FLOAT,
                          GL_FALSE,
                          VERTEX_STRIDE * sizeof(float),
                          (void *)(5 * sizeof(float)));
    // normal
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3,
                          1,
                          GL_FLOAT,
                          GL_FALSE,
                          VERTEX_STRIDE * sizeof(float),
                          (void *)(6 * sizeof(float)));

    glBindVertexArray(0);

    free(vertices);
    free(indices);

    //   build complete > allow render
    mesh->status = CSTATUS_READY;
}

void
chunkmesh_init(struct chunk *chunk)
{
    chunk->mesh       = malloc(sizeof(struct chunkmesh));
    chunk->mesh_trans = malloc(sizeof(struct chunkmesh));

    // SET VAO TO -1 to indicate lack of opengl buffers
    chunk->mesh->VAO       = -1;
    chunk->mesh_trans->VAO = -1;

    chunk->mesh->status       = CSTATUS_EMPTY;
    chunk->mesh_trans->status = CSTATUS_EMPTY;
}

struct chunk *
chunk_init(int ix, int iz, enum chunktype chunktype)
{
    struct chunk *out = malloc(sizeof(struct chunk));
    out->data         = calloc(CHUNK_VOLUME, sizeof(int));
    out->lightdata    = calloc(CHUNK_VOLUME, sizeof(int));

    memset(out->data, 0, CHUNK_VOLUME);
    memset(out->lightdata, 0, CHUNK_VOLUME);
    out->num_lights  = 0;
    out->max_lights  = 16;
    out->lightstatus = CSTATUS_EMPTY;
    out->lights      = malloc(out->max_lights * sizeof(struct light));
    switch (chunktype)
    {
        case CHUNKTYPE_NORMAL:

            chunk_xzforeach()
            {
                float gen = worldgen_perlin_fbm_2d(x + ix * CHUNK_SIZE,
                                                   z + iz * CHUNK_SIZE,
                                                   5562,
                                                   3,
                                                   1.0f,
                                                   0.45f);

                float height = gen * (CHUNK_HEIGHT - 1);

                chunk_column_fill(out, x, height, z);
            }

            chunk_foreach()
            {
                if (y > WATER_LEVEL)
                    continue;

                if (chunk_get_block(out, x, y, z) == BLOCK_AIR)
                {
                    chunk_set_block(out, BLOCK_WATER, x, y, z);
                }
            }

            break;

        case CHUNKTYPE_EMPTY:

            break;
    }

    out->x = ix;
    out->z = iz;

    chunkmesh_init(out);

    return out;
}

float
chunk_dist(int x, int z)
{
    int px, pz;
    camera_get_chunk(&STATE.cam_main, &px, &pz);

    float dist = SQUARE((float)px - (float)x) + SQUARE((float)pz - (float)z);
    return dist;
}

void
chunkmesh_destroy(struct chunkmesh *mesh)
{
    if (mesh && mesh->VAO != -1)
    {
        glDeleteBuffers(1, &mesh->VBO);
        glDeleteBuffers(1, &mesh->EBO);
        glDeleteVertexArrays(1, &mesh->VAO);
    }

    mesh->VAO = -1;
}

void
chunk_destroy(struct chunk *chunk)
{
    chunkmesh_destroy(chunk->mesh);
    chunkmesh_destroy(chunk->mesh_trans);

    for (int i = 0; i < chunk->num_lights; i++)
    {
        free(chunk->lights[i]);
    }

    free(chunk->mesh);
    free(chunk->mesh_trans);
    free(chunk->lightdata);
    free(chunk->lights);
    free(chunk->data);
    free(chunk);
}

void
chunks_destroy(struct chunk **chunks, int num)
{
    for (int i = 0; i < num; i++)
    {
        chunk_destroy(chunks[i]);
    }
}
