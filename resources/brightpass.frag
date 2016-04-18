#version 330 core

uniform sampler2D color_texture;

out vec4 color;

in VertexData {
    vec2 tex_coord;
} vertOut;

void main()
{
    vec4 tex_color = texture(color_texture, vertOut.tex_coord);

    float brightness = dot(tex_color.rgb, vec3(0.2126, 0.7152, 0.0722));

    if (brightness > 1.0) {
        color = tex_color;
    } else {
        color = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
