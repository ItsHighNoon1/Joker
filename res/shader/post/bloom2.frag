#version 330 core

in vec2 v_texCoords;

out vec4 a_fragColor;

uniform sampler2D u_currentColor;
uniform sampler2D u_sceneColor;

const float bloomIntensity = 1.0;

void main(void) {
	a_fragColor = texture(u_sceneColor, v_texCoords) + bloomIntensity * texture(u_currentColor, v_texCoords);
}