#version 150 core

uniform sampler2D color_texture;

out vec4 out_color;

in VertexData {
    vec3 position;
    vec2 tex_coord;
    vec3 normal;
} vertOut;

void main()
{
    //out_color = vec4(vert_tex_coord, 1.f, 1.f);
    out_color = texture(color_texture, vertOut.tex_coord);
}
