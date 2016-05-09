uniform sampler2D color_texture;
uniform sampler2D bloomed_texture;

layout(std140) uniform HDRData {
    float exposure;
    bool bloom_horizontal;
} hdr;

out vec4 color;

in VertexData {
    vec2 tex_coord;
} vertOut;

void main()
{
    vec3 hdrColor = texture(color_texture, vertOut.tex_coord).rgb;
    vec3 bloomColor = texture(bloomed_texture, vertOut.tex_coord).rgb;
    hdrColor += bloomColor;
    
    vec3 mapped = vec3(1.0) - exp(-hdrColor * hdr.exposure);
    
    color = vec4(mapped, 1.0);
}
