#version 330 core

layout(location = 0) in vec3 a_position;

out vec3 v_texCoords;

uniform mat4 u_viewProjectionMatrix;

void main(void) {
	gl_Position = u_viewProjectionMatrix * vec4(3.0 * a_position, 1.0);
	v_texCoords = a_position;
}