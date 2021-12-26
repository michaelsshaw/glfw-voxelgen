#version 330 core

out vec4 frag_color;

in vec2  tex_coord;
in float light;
in float lightvalue;

uniform sampler2D tex;

uniform int texture_pos;
uniform int time;

void
main()
{
    float ltime = (sin(float(time) / 1200.0f) + 1) * 16.0f;
	frag_color = texture(tex, tex_coord) * (clamp(ltime + lightvalue, 0, 15.9) / 16.0f + 0.1f);
}
