
layout(std140) uniform ViewData {
    mat4 projection_matrix;
    mat4 view_matrix;
    mat4 view_projection_matrix;
} view;

in vec3 in_position;
in float in_radius;

out VertexData {
    float radius;
} vertOut;

void main() {
	gl_Position = view.view_matrix * vec4(in_position, 1.f);
	vertOut.radius = in_radius;
}
