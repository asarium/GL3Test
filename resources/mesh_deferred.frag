#version 330 core

layout (location = 0) out vec3 out_position;
layout (location = 1) out vec3 out_normal;
layout (location = 2) out vec4 out_albedo;

uniform sampler2D color_texture;
uniform vec2 window_size;

in vec3 vert_position;
in vec2 vert_tex_coord;
in vec3 vert_normal;

void main()
{
    out_position = vert_position;
    out_normal = normalize(vert_normal);
    out_albedo = texture(color_texture, vert_tex_coord);
}
