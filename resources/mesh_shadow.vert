
layout(std140) uniform ViewData {
    mat4 projection_matrix;
    mat4 view_matrix;
    mat4 view_projection_matrix;
} view;

layout(std140) uniform ModelData {
    mat4 model_matrix;
    mat4 normal_model_matrix;
} push;

in vec3 in_position;

void main()
{
    gl_Position = view.view_projection_matrix * push.model_matrix * vec4(in_position, 1.f);
}
