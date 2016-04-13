#version 330 core

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo;

uniform int light_has_shadow;
uniform sampler2DShadow directional_shadow_map;
uniform mat4 light_proj_matrix;
uniform mat4 light_view_matrix;

uniform vec2 window_size;
uniform vec2 uv_scale;

uniform int light_type;
uniform vec3 light_vector;
uniform vec3 light_color;
uniform float light_intensity;

out vec4 frag_color;

float calculate_directed_shadow(vec3 position) {
    if (light_has_shadow == 0) {
        return 1.f;
    }

    vec4 pos_light_space = light_proj_matrix * light_view_matrix * vec4(position, 1.f);

    vec3 proj_pos = pos_light_space.xyz / pos_light_space.w;
    proj_pos = (proj_pos * 0.5f) + vec3(0.5f);
    proj_pos.z -= 0.01;

    return texture(directional_shadow_map, proj_pos);
}

void main()
{
    vec2 tex_coord = gl_FragCoord.xy / window_size * uv_scale;

    // Get gBuffer data
    vec3 position = texture(g_position, tex_coord).xyz;
    vec3 normal = texture(g_normal, tex_coord).xyz;
    vec3 color = texture(g_albedo, tex_coord).rgb;

    vec3 lighting = vec3(0.f);
    if (light_type == 0)
    {
        // Point light
        vec3 diff = light_vector - position;
        vec3 L = normalize(diff);

        float intens = max(dot(L, normal), 0.f);
        lighting = color * ((light_color * intens) / (1 + dot(diff, diff)));
    }
    else if (light_type == 1)
    {
        // Directional light
        float intens = max(dot(light_vector, normal), 0.f);
        lighting = light_color * color * intens * calculate_directed_shadow(position);
    }
    else if (light_type == 2)
    {
        // Ambient light
        lighting = light_color * color;
    }
    frag_color = vec4(lighting, 1.f);

/*
    if (gl_FragCoord.x < 0.f) {
        if (gl_FragCoord.y > 0.f) {
            vec2 adjusted_coord = vec2(vert_tex_coord.x * 2.f, vert_tex_coord.y * 2.f - 1.f);
            frag_color = texture(g_position, adjusted_coord);
        } else {
            vec2 adjusted_coord = vec2(vert_tex_coord.x * 2.f, vert_tex_coord.y * 2.f);
            frag_color = texture(g_albedo, adjusted_coord);
        }
    } else {
        if (gl_FragCoord.y > 0.f) {
            vec2 adjusted_coord = vec2(vert_tex_coord.x * 2.f - 1.f, vert_tex_coord.y * 2.f - 1.f);
            frag_color = texture(g_normal, adjusted_coord);
        } else {
            frag_color = vec4(0.f);
        }
    }
    */
}
