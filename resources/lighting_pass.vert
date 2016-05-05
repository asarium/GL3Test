uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

uniform mat4 light_proj_matrix;
uniform mat4 light_view_matrix;

uniform int light_type;

in vec3 in_position;

void main()
{
    if (light_type == 0) {
        // Point lights use a sphere geometry
        gl_Position = proj_matrix * view_matrix * model_matrix * vec4(in_position, 1.f);
    } else {
        gl_Position = vec4(in_position, 1.f);
    }
}
