#version 330 core

in vec2 v_texCoords;

out vec4 a_fragColor;

uniform sampler2D u_currentColor;

const float unitDist = 0.001;

void main(void) {
	a_fragColor = vec4(0.0);
	a_fragColor += 0.00598 * texture(u_currentColor, vec2(v_texCoords.x - unitDist * 3, v_texCoords.y));
	a_fragColor += 0.060626 * texture(u_currentColor, vec2(v_texCoords.x - unitDist * 2, v_texCoords.y));
	a_fragColor += 0.241843 * texture(u_currentColor, vec2(v_texCoords.x - unitDist, v_texCoords.y));
	a_fragColor += 0.383103 * texture(u_currentColor, vec2(v_texCoords.x, v_texCoords.y));
	a_fragColor += 0.241843 * texture(u_currentColor, vec2(v_texCoords.x + unitDist, v_texCoords.y));
	a_fragColor += 0.060626 * texture(u_currentColor, vec2(v_texCoords.x + unitDist * 2, v_texCoords.y));
	a_fragColor += 0.00598 * texture(u_currentColor, vec2(v_texCoords.x + unitDist * 3, v_texCoords.y));
}