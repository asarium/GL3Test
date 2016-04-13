#version 330 core

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo;

uniform vec2 window_size;
uniform vec2 uv_scale;

uniform int light_type;
uniform vec3 light_vector;
uniform vec3 light_color;
uniform float light_intensity;

out vec4 frag_color;

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
        lighting = light_color * color * intens;
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
