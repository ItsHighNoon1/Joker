#version 400 core

in vec2 v_texCoords;

out vec4 a_fragColor;

uniform sampler2D u_tex;

void main(void) {
	a_fragColor = texture(u_tex, v_texCoords);
	if (a_fragColor.a < 0.5) {
		discard;
	}
}