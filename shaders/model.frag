#version 150

out vec4 out_Color;
in vec4 vert_position;

void main()
{
    out_Color = vec4(vert_position.xyz, 1.f);
}
