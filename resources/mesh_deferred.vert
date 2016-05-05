uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

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
    gl_Position = proj_matrix*view_matrix*model_matrix*in_position;

    vertOut.position = (model_matrix*in_position).xyz;
    vertOut.tex_coord = in_tex_coord;
    vertOut.normal = transpose(inverse(mat3(model_matrix))) * in_normal;
}
