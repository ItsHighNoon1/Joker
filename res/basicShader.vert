#version 400 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texCoords;
layout(location = 2) in vec3 a_normal;

out vec3 v_surfaceNormal;
out vec3 v_toLightVector;
out vec2 v_texCoords;

uniform mat4 u_modelViewProjectionMatrix; // Used for vertex transformation
uniform mat4 u_modelMatrix; // Used exclusively for lighting calculations
uniform vec3 u_lightDirection;

void main(void) {
	// Position computations
	gl_Position = u_modelViewProjectionMatrix * vec4(a_position, 1.0);

	// Texture coordinate computations
	v_texCoords = a_texCoords;

	// Normal computations
	v_toLightVector = -u_lightDirection; // For point lighting later on
	v_surfaceNormal = (u_modelMatrix * vec4(a_normal, 0.0)).xyz;
}