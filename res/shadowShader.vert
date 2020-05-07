#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texCoords;

out vec2 v_texCoords;

uniform mat4 u_modelShadowMatrix;

void main(void) {
	gl_Position = u_modelShadowMatrix * vec4(a_position, 1.0);
	v_texCoords = a_texCoords;
}