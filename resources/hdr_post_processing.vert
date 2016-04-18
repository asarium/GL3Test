#version 330 core

in vec3 in_position;
in vec2 in_tex_coord;

out VertexData {
    vec2 tex_coord;
} vertOut;

void main() {
    gl_Position = vec4(in_position, 1.f);

    vertOut.tex_coord = in_tex_coord;
}
