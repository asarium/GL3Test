const vec2 corners[4] = vec2[]( vec2(-1.0, 1.0), vec2(-1.0, -1.0), vec2(1.0, 1.0), vec2(1.0, -1.0) );

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 proj_matrix;

in VertexData {
    float radius;
} vertOut[];

out VertexData {
    vec2 tex_coord;
} geomOut;

void main()
{
    for(int i=0; i<4; ++i)
    {
        vec4 eyePos = gl_in[0].gl_Position;           //start with point position
        eyePos.xy += vertOut[0].radius * corners[i];         //add corner position
        gl_Position = proj_matrix * eyePos;             //complete transformation
        geomOut.tex_coord = corners[i];                         //use corner as texCoord
        EmitVertex();
    }
    EndPrimitive();
}
