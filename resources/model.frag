#version 150 core

uniform sampler2D color_texture;
uniform vec2 window_size;

out vec4 out_color;

in vec4 vert_position;
in vec2 vert_tex_coord;
in vec3 vert_normal;

void main()
{
    //out_color = vec4(vert_tex_coord, 1.f, 1.f);
    out_color = texture(color_texture, vert_tex_coord);
}
