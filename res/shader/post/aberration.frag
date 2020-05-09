#version 330 core

in vec2 v_texCoords;

out vec4 a_fragColor;

uniform sampler2D u_currentColor;

void main(void) {
	a_fragColor.x = texture(u_currentColor, v_texCoords - 0.01).x;
	a_fragColor.y = texture(u_currentColor, v_texCoords).y;
	a_fragColor.z = texture(u_currentColor, v_texCoords + 0.01).z;
}