
layout(std140) uniform ViewData {
    mat4 projection_matrix;
    mat4 view_matrix;
    mat4 view_projection_matrix;
} view;

in vec3 in_position;
in vec2 in_tex_coord;

in vec3 in_pos_offset;
in float in_radius;

out VertexData {
    vec2 tex_coord;
} vertOut;

void main() {
    vec4 view_pos = view.view_matrix * vec4(in_pos_offset, 1.f);
    view_pos.xy += (in_position * in_radius).xy;

    gl_Position = view.projection_matrix * view_pos;

    vertOut.tex_coord = in_tex_coord;
}
