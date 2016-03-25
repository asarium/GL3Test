#version 150

out vec4 out_Color;

in vec2 vert_tex_coord;

void main()
{
    out_Color = vec4(vert_tex_coord.xy, 0.f, 1.f);
}
