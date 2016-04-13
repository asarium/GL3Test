#version 330 core

uniform mat4 proj_matrix;
uniform mat4 view_matrix;

in vec3 in_position;
in vec2 in_tex_coord;

in vec3 in_pos_offset;
in float in_radius;

out VertexData {
    vec2 tex_coord;
} vertOut;

void main() {
    vec4 view_pos = view_matrix * vec4(in_pos_offset, 1.f);
    view_pos.xy += (in_position * in_radius).xy;

    gl_Position = proj_matrix * view_pos;

    vertOut.tex_coord = in_tex_coord;
}
