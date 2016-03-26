#version 150

uniform sampler2D color_texture;
uniform vec2 window_size;

out vec4 out_color;

in vec2 vert_tex_coord;

void main()
{
    out_color = vec4(vert_tex_coord, 1.f, 1.f);
}
