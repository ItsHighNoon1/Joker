#version 330 core

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_texCoords;

out vec2 v_texCoords;

void main(void) {
	// All post processing is just a fragment shader on a quad
	gl_Position = vec4(a_position, 0.0, 1.0);
	v_texCoords = a_texCoords;
}