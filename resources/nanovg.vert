
layout(std140) uniform NanoVGGlobalData {
    vec2 viewSize;
};

in vec2 in_position_2d;
in vec2 in_tex_coord;

out vec2 ftcoord;
out vec2 fpos;

void main(void) {
	ftcoord = in_tex_coord;
	fpos = in_position_2d;
	gl_Position = vec4(2.0*in_position_2d.x/viewSize.x - 1.0, 1.0 - 2.0*in_position_2d.y/viewSize.y, 0, 1);
}
