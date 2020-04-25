#version 400 core

layout(location = 0) in vec3 position;

out vec3 color;

uniform mat4 modelViewProjectionMatrix;

void main(void) {
	gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);
	color = vec3(position.x + 0.5, sin(gl_Position.z), position.y + 0.5);
}