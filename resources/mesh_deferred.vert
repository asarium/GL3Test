
layout(std140) uniform ViewData {
    mat4 projection_matrix;
    mat4 view_matrix;
    mat4 view_projection_matrix;
} view;

layout(std140) uniform ModelData {
    mat4 model_matrix;
    mat4 normal_model_matrix;
} model;

in vec4 in_position;
in vec2 in_tex_coord;
in vec3 in_normal;

out VertexData {
    vec3 position;
    vec2 tex_coord;
    vec3 normal;
} vertOut;

void main()
{
    gl_Position = view.view_projection_matrix * model.model_matrix * in_position;

    vertOut.position = (model.model_matrix * in_position).xyz;
    vertOut.tex_coord = in_tex_coord;
    vertOut.normal = normalize(model.normal_model_matrix * vec4(in_normal, 0.f)).xyz;
}
