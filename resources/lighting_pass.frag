#version 330 core

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo;

uniform int light_type;
uniform vec3 light_vector;
uniform vec3 light_color;
uniform float light_intensity;

in vec3 vert_position;
in vec2 vert_tex_coord;

out vec4 frag_color;

void main()
{
    // Get gBuffer data
    vec3 position = texture(g_position, vert_tex_coord).xyz;
    vec3 normal = texture(g_normal, vert_tex_coord).xyz;
    vec3 color = texture(g_albedo, vert_tex_coord).rgb;

    vec3 lighting = vec3(0.f);
    if (light_type == 0)
    {
        // Point light
        vec3 diff = light_vector - position;
        vec3 L = normalize(diff);

        float intens = max(dot(L, normal), 0.f);
        lighting = (light_intensity * light_color * color * intens) / (dot(diff, diff));
    }
    frag_color = vec4(lighting, 1.f);

/*
    if (vert_position.x < 0.f) {
        if (vert_position.y > 0.f) {
            vec2 adjusted_coord = vec2(vert_tex_coord.x * 2.f, vert_tex_coord.y * 2.f - 1.f);
            frag_color = texture(g_position, adjusted_coord);
        } else {
            vec2 adjusted_coord = vec2(vert_tex_coord.x * 2.f, vert_tex_coord.y * 2.f);
            frag_color = texture(g_albedo, adjusted_coord);
        }
    } else {
        if (vert_position.y > 0.f) {
            vec2 adjusted_coord = vec2(vert_tex_coord.x * 2.f - 1.f, vert_tex_coord.y * 2.f - 1.f);
            frag_color = texture(g_normal, adjusted_coord);
        } else {
            frag_color = vec4(0.f);
        }
    }
    */
}
