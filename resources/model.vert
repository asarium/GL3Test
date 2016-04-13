#version 150 core

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

in vec3 in_position;
in vec2 in_tex_coord;
in vec3 in_normal;

out VertexData {
    vec3 position;
    vec2 tex_coord;
    vec3 normal;
} vertOut;

void main()
{
    gl_Position = proj_matrix*view_matrix*model_matrix*vec4(in_position, 1.f);

    vertOut.position = in_position;
    vertOut.tex_coord = in_tex_coord;
    vertOut.normal = normalize((model_matrix * vec4(in_normal, 0.f)).xyz);
}
