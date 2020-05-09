#version 330 core

in vec2 v_texCoords;

out vec4 a_fragColor;

uniform sampler2D u_color;

void main(void) {
	a_fragColor = texture(u_color, v_texCoords);
}