#version 330 core

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

in vec4 in_position;
in vec2 in_tex_coord;
in vec3 in_normal;

out vec3 vert_position;
out vec2 vert_tex_coord;
out vec3 vert_normal;

void main()
{
    gl_Position = proj_matrix*view_matrix*model_matrix*in_position;

    vert_position = (model_matrix*in_position).xyz;
    vert_tex_coord = in_tex_coord;
    vert_normal = normalize((model_matrix * vec4(in_normal, 0.f)).xyz);
}
