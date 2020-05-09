#version 330 core

in vec2 v_texCoords;

out vec4 a_fragColor;

uniform sampler2D u_currentColor;

void main(void) {
	vec4 color = texture(u_currentColor, v_texCoords);
	float brightness = 0.2126 * color.x + 0.7152 * color.y + 0.0722 * color.z;
	a_fragColor = brightness * brightness * brightness * color;
}