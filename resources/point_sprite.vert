#version 330 core

uniform mat4 view_matrix;

in vec3 in_position;
in float in_radius;

out VertexData {
    float radius;
} vertOut;

void main() {
	gl_Position = view_matrix * vec4(in_position, 1.f);
	vertOut.radius = in_radius;
}
