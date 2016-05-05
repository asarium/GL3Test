uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

in vec3 in_position;


void main()
{
    gl_Position = proj_matrix * view_matrix * model_matrix * vec4(in_position, 1.f);
}
