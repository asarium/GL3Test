uniform sampler2D color_texture;

layout(std140) uniform PushConstants {
    float exposure;
    bool bloom_horizontal;
} push;

const float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

out vec4 color;

in VertexData {
    vec2 tex_coord;
} vertOut;

void main()
{             
    vec2 tex_offset = 1.0 / textureSize(color_texture, 0); // gets size of single texel
    vec3 result = texture(color_texture, vertOut.tex_coord).rgb * weight[0]; // current fragment's contribution

    if(push.bloom_horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(color_texture, vertOut.tex_coord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(color_texture, vertOut.tex_coord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(color_texture, vertOut.tex_coord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(color_texture, vertOut.tex_coord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }

    color = vec4(result, 1.0);
}
