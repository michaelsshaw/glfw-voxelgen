#define STB_IMAGE_IMPLEMENTATION

#include <cglm/cglm.h>
#include <glad/glad.h>
#include <math.h>
#include <stb/stb_image.h>

#include "../state.h"
#include "render.h"

#include "../world/block.h"
#include "../world/chunk.h"

unsigned int
texture_create(const char *file_name,
			   GLenum	   color_format,
			   GLenum	   wrap_st,
			   GLenum	   filter_minmag)
{
	int width, height, channels;

	stbi_set_flip_vertically_on_load(1);
	unsigned char *data = stbi_load(file_name, &width, &height, &channels, 0);
	unsigned int   texture;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D,
				 0,
				 color_format,
				 width,
				 height,
				 0,
				 color_format,
				 GL_UNSIGNED_BYTE,
				 data);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_st);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_st);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_minmag);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_minmag);

	stbi_image_free(data);

	return texture;
}

void
chunk_render(struct chunk *chunk)
{
	if (chunk->mesh->status != CSTATUS_READY &&
		chunk->mesh->status != CSTATUS_OUTDATED)
	{
		return;
	}

	{
		vec3 trans = { chunk->x, 0, chunk->z };
		mat4 model = GLM_MAT4_IDENTITY_INIT;
		glm_translate(model, trans);

		shader_uniform_mat4(STATE.shader_block, "model_matrix", *model);
		shader_uniform_int(STATE.shader_block, "tex", 0);

        shader_uniform_int(STATE.shader_block, "time", STATE.daytime);

		glBindVertexArray(chunk->mesh->VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk->mesh->EBO);
		glDrawElements(
		  GL_TRIANGLES, chunk->mesh->triangle_count * 3, GL_UNSIGNED_INT, 0);
	}

	if (chunk->mesh_trans->status != CSTATUS_READY &&
		chunk->mesh_trans->status != CSTATUS_OUTDATED)
	{
		return;
	}

	{
		vec3 trans = { chunk->x, 0, chunk->z };
		mat4 model = GLM_MAT4_IDENTITY_INIT;
		glm_translate(model, trans);

		shader_uniform_mat4(STATE.shader_block, "model_matrix", *model);
		shader_uniform_int(STATE.shader_block, "tex", 0);

        shader_uniform_int(STATE.shader_block, "time", STATE.daytime);

		glBindVertexArray(chunk->mesh_trans->VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk->mesh_trans->EBO);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDrawElements(GL_TRIANGLES,
					   chunk->mesh_trans->triangle_count * 3,
					   GL_UNSIGNED_INT,
					   0);
		glDisable(GL_BLEND);
	}
}
