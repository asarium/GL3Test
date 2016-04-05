#version 330 core

const vec2 corners[4] = vec2[]( vec2(0.0, 1.0), vec2(0.0, 0.0), vec2(1.0, 1.0), vec2(1.0, 0.0) );

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 proj_matrix;

in float vRadius[];

out vec2 vTexCoord;

void main()
{
    for(int i=0; i<4; ++i)
    {
        vec4 eyePos = gl_in[0].gl_Position;           //start with point position
        eyePos.xy += vRadius[0] * (corners[i] - vec2(0.5)); //add corner position
        gl_Position = proj_matrix * eyePos;             //complete transformation
        vTexCoord = corners[i];                         //use corner as texCoord
        EmitVertex();
    }
    EndPrimitive();
}