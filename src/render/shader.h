#ifndef SHADER_H
#define SHADER_H

#define SHADER_DIRECTORY "shaders/"

unsigned int
shader_create(char *name);

void
shader_uniform_mat4(unsigned int shader, const char *name, float *i_mat);
void
shader_uniform_vec2(unsigned int shader, const char *name, float *i_vec);
void
shader_uniform_vec2v(unsigned int shader, const char *name, float x, float y);
void
shader_uniform_vec3(unsigned int shader, const char *name, float *i_vec);
void
shader_uniform_vec3v(unsigned int shader,
                     const char  *name,
                     float        x,
                     float        y,
                     float        z);
void
shader_uniform_float(unsigned int shader, const char *name, float x);
void
shader_uniform_int(unsigned int shader, const char *name, int x);

#endif // SHADER_H
