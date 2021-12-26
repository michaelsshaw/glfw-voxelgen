#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_texcoord;
layout (location = 2) in float a_light;
layout (location = 3) in float a_lightvalue;

out vec2 tex_coord;
out float light;
out float lightvalue;

uniform mat4 cam_matrix;
uniform mat4 model_matrix;

void main()
{
	
	gl_Position = cam_matrix * model_matrix * vec4(a_pos, 1.0);
	
	lightvalue = a_lightvalue;
	tex_coord = a_texcoord;
	light = a_light;
}