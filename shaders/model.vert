#version 150

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

in vec4 in_position;

out vec4 vert_position;

void main()
{
    gl_Position = proj_matrix*view_matrix*model_matrix*in_position;
    vert_position = in_position;
}
