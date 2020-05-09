#version 330 core

in vec2 v_texCoords;

out vec4 a_fragColor;

uniform sampler2D u_currentColor;

void main(void) {
	float radius = length(v_texCoords - 0.5);
	vec2 samplingLocation = vec2(v_texCoords.x + radius * sin(v_texCoords.y), v_texCoords.y + radius * sin(v_texCoords.x));
	a_fragColor = texture(u_currentColor, samplingLocation);
}