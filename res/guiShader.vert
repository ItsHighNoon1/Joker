#version 400 core

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_texCoords;

out vec2 v_texCoords;

uniform mat4 u_transformationMatrix;

void main(void) {
	gl_Position = u_transformationMatrix * vec4(a_position, 0.0, 1.0);
	v_texCoords = a_texCoords;
}