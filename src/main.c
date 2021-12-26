#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <math.h>
#include <stdio.h>

#include "render/render.h"
#include "state.h"
#include "game.h"

struct state STATE;

void
error(const char *err)
{
    printf("ERROR::%s\n", err);
    exit(1);
}

void
glfw_error(int err, const char *desc)
{
    error(desc);
}

void
framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);

    STATE.cam_main.width  = width;
    STATE.cam_main.height = height;

    STATE.window_width  = width;
    STATE.window_height = height;
}

int
main(int argc, char *args[])
{
    /*
     * Create the window and renderer
     */

    STATE.window_width  = 800;
    STATE.window_height = 800;
    STATE.tick_rate     = 60;

    STATE.window_title = "Minecraft";

    glfwSetErrorCallback(glfw_error);

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(
      STATE.window_width, STATE.window_height, STATE.window_title, NULL, NULL);
    if (window == NULL)
    {
        error("GLFW failed to create window");
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetWindowSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    gladLoadGL();
    glEnable(GL_DEPTH_TEST);

    game_begin(&STATE, window);

    return 0;
}
