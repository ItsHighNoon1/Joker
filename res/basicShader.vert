#version 400 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texCoords;

out vec2 v_texCoords;

uniform mat4 modelViewProjectionMatrix;

void main(void) {
	gl_Position = modelViewProjectionMatrix * vec4(a_position, 1.0); // Traditional, nothing to see here
	v_texCoords = a_texCoords; // Fragment shader needs tex coords
}