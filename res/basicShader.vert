#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texCoords;
layout(location = 2) in vec3 a_normal;

out vec4 v_fragPosLightSpace;
out vec3 v_surfaceNormal;
out vec3 v_toLightVector;
out vec2 v_texCoords;

uniform mat4 u_modelViewProjectionMatrix; // Used for vertex transformation
uniform mat4 u_modelMatrix; // Used exclusively for lighting calculations
uniform mat4 u_modelShadowMatrix;
uniform vec3 u_lightDirection;
uniform vec2 u_texOffset;
uniform int u_texRows;

void main(void) {
	// Position computations
	gl_Position = u_modelViewProjectionMatrix * vec4(a_position, 1.0);

	// Texture coordinate computations
	if (u_texRows <= 1) {
		v_texCoords = a_texCoords;
	} else {
		v_texCoords = (a_texCoords / u_texRows) + u_texOffset;
	} 

	// Normal computations
	v_toLightVector = -u_lightDirection; // For point lighting later on
	v_surfaceNormal = (u_modelMatrix * vec4(a_normal, 0.0)).xyz;

	// Shadow computations
	v_fragPosLightSpace = u_modelShadowMatrix * vec4(a_position, 1.0);
}