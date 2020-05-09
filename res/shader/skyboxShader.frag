#version 330 core

in vec3 v_texCoords;

out vec4 a_fragColor;

uniform samplerCube u_tex;

void main(void) {
	a_fragColor = texture(u_tex, v_texCoords);
}