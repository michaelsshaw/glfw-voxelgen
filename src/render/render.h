#ifndef RENDER_H
#define RENDER_H

#include "../world/block.h"
#include "../world/chunk.h"
#include "shader.h"
#include <cglm/cglm.h>
#include <glad/glad.h>

unsigned int
texture_create(const char *file_name,
               GLenum      color_format,
               GLenum      wrap_st,
               GLenum      filter_minmag);

void
chunk_render(struct chunk *chunk);

#endif // RENDER_H
