#include "camera.h"
#include "../state.h"
#include "shader.h"
#include <GLFW/glfw3.h>
#include <math.h>

float yaw	= 0.0f;
float pitch = 0.0f;

void
camera_matrix(float			 deg_fov,
			  float			 near_plane,
			  float			 far_plane,
			  unsigned int	 shader,
			  const char	 *uniform,
			  struct camera *i_camera)
{
	mat4 view = GLM_MAT4_IDENTITY_INIT;
	mat4 proj = GLM_MAT4_IDENTITY_INIT;

	vec3 position_orientation;
	glm_vec3_add(i_camera->position,	// v1
				 i_camera->orientation, // v2
				 position_orientation); // destination

	glm_lookat(i_camera->position, position_orientation, GLM_YUP, view);
	float aspect = (float)i_camera->width / (float)i_camera->height;
	glm_perspective(glm_rad(deg_fov), aspect, near_plane, far_plane, proj);

	mat4 out;

	glm_mat4_mul(proj, view, out);

	shader_uniform_mat4(shader, uniform, (float *)out);
}

int	   i_pressed = 0;
GLenum mode		 = GL_LINE;
void
camera_process_input(GLFWwindow *window, struct camera *i_cam, float d_time)
{
	const float cam_speed = 0.05f * d_time; // adjust accordingly

	vec3 o_move;

	// FORWARD
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		vec3 dir;
		glm_vec3_copy(i_cam->orientation, dir);
		glm_vec3_scale(dir, cam_speed, dir);
		glm_vec3_add(i_cam->position, dir, i_cam->position);
	}

	// BACKWARD
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		vec3 dir;
		glm_vec3_copy(i_cam->orientation, dir);
		glm_vec3_scale(dir, cam_speed, dir);
		glm_vec3_sub(i_cam->position, dir, i_cam->position);
	}

	// LEFT
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		vec3 dir;

		glm_vec3_cross(i_cam->orientation, GLM_YUP, dir);
		glm_normalize(dir);
		glm_vec3_scale(dir, cam_speed, dir);

		glm_vec3_sub(i_cam->position, dir, i_cam->position);
	}

	// RIGHT
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		vec3 dir;

		glm_vec3_cross(i_cam->orientation, GLM_YUP, dir);
		glm_normalize(dir);
		glm_vec3_scale(dir, cam_speed, dir);

		glm_vec3_add(i_cam->position, dir, i_cam->position);
	}

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !i_pressed)
	{
		i_pressed = 1;
		if (mode == GL_LINE)
			mode = GL_FILL;
		else
			mode = GL_LINE;
		glPolygonMode(GL_FRONT_AND_BACK, mode);
	}

	if (glfwGetKey(window, GLFW_KEY_F) != GLFW_PRESS && i_pressed)
	{
		i_pressed = 0;
	}

	// MOUSE MOVEMENT
	if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
	{
		double mouse_x;
		double mouse_y;

		glfwGetCursorPos(window, &mouse_x, &mouse_y);

		float sens = 30.0f;
		float rotx =
		  sens * (float)(mouse_y - (i_cam->height / 2)) / i_cam->height;
		float roty =
		  sens * (float)(mouse_x - (i_cam->width / 2)) / i_cam->height;

		vec3 orientation_new;
		vec3 yaxis;

		glm_vec3_cross(i_cam->orientation, GLM_YUP, yaxis);
		glm_normalize(yaxis);

		glm_vec3_copy(i_cam->orientation, orientation_new);

		glm_vec3_rotate(orientation_new, glm_rad(-rotx), yaxis);

		if (glm_vec3_angle(orientation_new, GLM_YUP) >= glm_rad(5.0f) &&
			glm_vec3_angle(orientation_new, GLM_YUP) <= glm_rad(175.0f))
		{
			glm_vec3_copy(orientation_new, i_cam->orientation);
		}

		glm_vec3_rotate(i_cam->orientation, glm_rad(-roty), GLM_YUP);

		glfwSetCursorPos(window, i_cam->width / 2, i_cam->height / 2);
	}
	else // REENTER WINDOW
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			glfwSetCursorPos(window, i_cam->width / 2, i_cam->height / 2);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

	// ESCAPE WINDOW
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		world_reset_all(&STATE.world);
	}
}

void
camera_get_chunk(struct camera *cam, int *x, int *z)
{
	/* When dividing a negative integer by CHUNK_SIZE,
		it rounds up to 0, rather than down to -1, so we
		subtract CHUNK_SIZE from it to offset all negative
		values down by 1 */

	int px = (int)cam->position[0];
	int pz = (int)cam->position[2];
	if (px < 0)
	{
		px -= CHUNK_SIZE;
	}
	if (pz < 0)
	{
		pz -= CHUNK_SIZE;
	}

	*x = px / CHUNK_SIZE;
	*z = pz / CHUNK_SIZE;
}