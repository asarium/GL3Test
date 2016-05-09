
layout(std140) uniform GlobalLightingData {
    vec2 window_size;
    vec2 uv_scale;
    mat4 view_projection;
} global;

layout(std140) uniform LightData {
    mat4 light_view_proj_matrix;
    mat4 model_matrix;

    vec3 light_vector;
    int light_type;
    
    vec3 light_color;
    bool light_has_shadow;
} light;

in vec3 in_position;

void main()
{
    if (light.light_type == 1) {
        // Point lights use a sphere geometry
        gl_Position = global.view_projection * light.model_matrix * vec4(in_position, 1.f);
    } else {
        gl_Position = vec4(in_position, 1.f);
    }
}
