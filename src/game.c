#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include "render/render.h"
#include "state.h"
#include "world/block.h"
#include "world/world.h"

float f_fov  = 75.0f;
float f_near = 0.1f;
float f_far  = 500.0f;

int
game_state(struct state *state, enum gamestate i_gamestate)
{
    state->gamestate = i_gamestate;
}

int
game_begin(struct state *state, GLFWwindow *window)
{
    game_state(state, STATE_MAINMENU);
    state->tex_atlas =
      texture_create("img/atlas.png", GL_RGBA, GL_REPEAT, GL_NEAREST);
    // LIGHT SHADER
    state->shader_block = shader_create("block");

    /*
     * CAMERA
     */
    vec3 cam_position    = { 0.0f, WATER_LEVEL + 10, 2.0f };
    vec3 cam_orientation = { 0.0f, 0.0f, -1.0f };

    state->cam_main.width  = STATE.window_width;
    state->cam_main.height = STATE.window_height;

    glm_vec3_copy(cam_position, state->cam_main.position);
    glm_vec3_copy(cam_orientation, state->cam_main.orientation);

    /*
     * IMPORTANT
     * GAME DATA INITIALIZATION
     */
    block_init();
    world_init();

    int running = 1;

    double ms_now      = glfwGetTime() * 1000.0f;
    double ms_last     = 0.0f;
    double ms_per_tick = 1.0f;
    double d_time      = 0.0f;

    int lastx = (int)(state->cam_main.position[0] / CHUNK_SIZE);
    int lastz = (int)(state->cam_main.position[2] / CHUNK_SIZE);

    int ms_elapsed;

    /* MAIN LOOP */
    while (!glfwWindowShouldClose(window))
    {
        ms_elapsed = 0;
        ms_now     = glfwGetTime() * 1000;
        /*
         * Rendering bit
         */
        glClearColor(0.2f, 0.5f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        d_time = ms_now - ms_last;

        int curx;
        int curz;
        camera_get_chunk(&state->cam_main, &curx, &curz);

        if (lastx != curx || lastz != curz)
        {
            world_chunk_check(&state->world, curx, curz);

            lastx = curx;
            lastz = curz;
        }

        camera_matrix(f_fov,
                      f_near,
                      f_far,
                      state->shader_block,
                      "cam_matrix",
                      &state->cam_main);

        camera_process_input(window, &state->cam_main, d_time);

        // chunks are loaded one at a time to prevent game lag
        ms_now = glfwGetTime() * 1000;
        while (ms_elapsed < 0)
        {
            ms_elapsed += glfwGetTime() * 1000 - ms_now;
        }

        world_chunk_loadnext(&state->world);
        world_chunk_rebuildnext(&state->world);
        /*
         * MAIN DRAW CALLS
         */

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, state->tex_atlas);
        glUseProgram(state->shader_block);
        for (int i = 0; i < state->world.num_chunks; i++)
        {
            if (!state->world.chunks[i])
                continue;
            chunk_render(state->world.chunks[i]);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (ms_now - ms_last >= ms_per_tick)
        {
            /*
             * Timed logic loop
             */

            STATE.daytime += 1;
        }

        ms_last = ms_now;
    }

    /*
     * Destroy the window
     */
    glfwDestroyWindow(window);
    glfwTerminate();
}
