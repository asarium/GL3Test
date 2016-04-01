#version 330 core

in vec3 in_position;
in vec2 in_tex_coord;

out vec3 vert_position;
out vec2 vert_tex_coord;

void main()
{
    gl_Position = vec4(in_position, 1.f);

    vert_position = in_position;
    vert_tex_coord = in_tex_coord;
}
