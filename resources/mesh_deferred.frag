layout (location = 0) out vec3 out_position;
layout (location = 1) out vec3 out_normal;
layout (location = 2) out vec4 out_albedo;

uniform sampler2D color_texture;

in VertexData {
    vec3 position;
    vec2 tex_coord;
    vec3 normal;
} vertOut;

void main()
{
    out_position = vertOut.position;
    out_normal = normalize(vertOut.normal);
    out_albedo = texture(color_texture, vertOut.tex_coord);
}
