uniform sampler2D color_texture;

out vec4 color;

in VertexData {
    vec2 tex_coord;
} vertOut;

void main()
{
    color = texture(color_texture, vertOut.tex_coord);
}
