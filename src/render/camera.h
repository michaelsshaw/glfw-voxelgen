#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

struct camera
{
    int  width;
    int  height;
    vec3 position;
    vec3 orientation;
};

void
camera_matrix(float          deg_fov,
              float          near_plane,
              float          far_plane,
              unsigned int   shader,
              const char    *uniform,
              struct camera *i_camera);

void
camera_process_input(GLFWwindow *window, struct camera *i_cam, float d_time);

void
camera_mouse(GLFWwindow *window, double xpos, double ypos);

void
camera_get_chunk(struct camera *cam, int *x, int *z);

#endif // CAMERA_H
