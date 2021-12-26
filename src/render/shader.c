#include "shader.h"
#include <cglm/cglm.h>
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *
shader_read(char *filename)
{
    char final[strlen(SHADER_DIRECTORY) + strlen(filename)];

    strcpy(final, SHADER_DIRECTORY);
    strcat(final, filename);

    FILE  *p_file;
    char  *buffer;
    size_t f_len, f_actualread;

    p_file = fopen(final, "r");

    fseek(p_file, 0L, SEEK_END);
    f_len = ftell(p_file);
    fseek(p_file, 0L, SEEK_SET);
    rewind(p_file);

    buffer = (char *)malloc(f_len + 1);

    f_actualread         = fread(buffer, 1, f_len, p_file);
    buffer[f_actualread] = 0;
    fclose(p_file);

    return buffer;
}

unsigned int
shader_create(char *name)
{
    char fs_name[strlen(name) + strlen(".fs") + 1];
    char vs_name[strlen(name) + strlen(".vs") + 1];

    strcpy(fs_name, name);
    strcpy(vs_name, name);

    strcat(fs_name, ".fs");
    strcat(vs_name, ".vs");

    // still not use as of yet
    const char *vertex_file   = shader_read(vs_name);
    const char *fragment_file = shader_read(fs_name);

    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_file, NULL);
    glCompileShader(vertex_shader);

    int  success = 0;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        printf("Vertex Shader compile error:\n%s\n", info_log);
    }
    success = 0;

    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_file, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        printf("Fragment shader compile error:\n%s\n", info_log);
    }
    success = 0;

    unsigned int shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        printf("Shader link error:\n%s\n", info_log);
    }

    return shader_program;
}

void
shader_uniform_mat4(unsigned int shader, const char *name, float *i_mat)
{
    glUseProgram(shader);
    int loc = glGetUniformLocation(shader, name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, i_mat);
}

void
shader_uniform_vec2(unsigned int shader, const char *name, float *i_vec)
{
    glUseProgram(shader);
    int loc = glGetUniformLocation(shader, name);
    glUniform2fv(loc, 1, i_vec);
}

void
shader_uniform_vec2v(unsigned int shader, const char *name, float x, float y)
{
    glUseProgram(shader);
    vec2 v = { x, y };
    shader_uniform_vec2(shader, name, v);
}

void
shader_uniform_vec3(unsigned int shader, const char *name, float *i_vec)
{
    glUseProgram(shader);
    int loc = glGetUniformLocation(shader, name);
    glUniform3fv(loc, 1, i_vec);
}

void
shader_uniform_vec3v(unsigned int shader,
                     const char  *name,
                     float        x,
                     float        y,
                     float        z)
{
    glUseProgram(shader);
    vec3 v = { x, y, z };
    shader_uniform_vec3(shader, name, v);
}

void
shader_uniform_float(unsigned int shader, const char *name, float x)
{
    glUseProgram(shader);
    int loc = glGetUniformLocation(shader, name);
    glUniform1f(loc, x);
}

void
shader_uniform_int(unsigned int shader, const char *name, int x)
{
    glUseProgram(shader);
    int loc = glGetUniformLocation(shader, name);
    glUniform1i(loc, x);
}
