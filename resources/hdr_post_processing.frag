uniform sampler2D color_texture;
uniform sampler2D bloomed_texture;

uniform float hdr_exposure;

out vec4 color;

in VertexData {
    vec2 tex_coord;
} vertOut;

void main()
{
    vec3 hdrColor = texture(color_texture, vertOut.tex_coord).rgb;
    vec3 bloomColor = texture(bloomed_texture, vertOut.tex_coord).rgb;
    hdrColor += bloomColor;
    
    vec3 mapped = vec3(1.0) - exp(-hdrColor * hdr_exposure);
    
    color = vec4(mapped, 1.0);
}
