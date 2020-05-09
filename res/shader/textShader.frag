#version 330 core

in vec2 v_texCoords;

out vec4 a_fragColor;

uniform vec3 u_textColor;
uniform sampler2D u_tex;

void main(void) {
	a_fragColor = vec4(u_textColor, 1.0) * texture(u_tex, v_texCoords);
	if (a_fragColor.a < 0.5) {
		discard;
	}
}