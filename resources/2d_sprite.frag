#version 330 core

uniform sampler2D color_texture;

out vec4 color;

in vec2 vTexCoord;

void main()
{
    color = texture(color_texture, vTexCoord);
}
