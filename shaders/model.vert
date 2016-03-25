#version 150

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

in vec4 in_position;
in vec2 in_tex_coord;

out vec4 vert_position;
out vec2 vert_tex_coord;

void main()
{
    gl_Position = proj_matrix*view_matrix*model_matrix*in_position;

    vert_position = in_position;
    vert_tex_coord = in_tex_coord;
}
