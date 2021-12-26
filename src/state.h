#ifndef STATE_H
#define STATE_H

#include "render/camera.h"
#include "world/world.h"
#include <semaphore.h>

#define TEX_ATLAS_SIZE 256
#define TEX_SIZE       16

#define SEM_CHUNKGEN_NAME "/sem_chunkgen"

enum gamestate
{
    STATE_MAINMENU,
    STATE_GAMEMENU,
    STATE_GAME
};

struct state
{
    unsigned int window_width;
    unsigned int window_height;
    unsigned int tick_rate;

    unsigned int tex_atlas;
    unsigned int shader_block;

    char *window_title;

    struct camera cam_main;
    struct world  world;

    // (side length) / 2 of a square around the player
    // chunks will be trimmed to this radius
    int world_radius;

    int x;
    int z;
    
    sem_t *sem_chunkgen;

    enum gamestate gamestate;

    unsigned int daytime;
};

extern struct state STATE;

#endif // STATE_H
