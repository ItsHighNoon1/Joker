#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texCoords;
layout(location = 2) in vec3 a_normal;

out vec4 v_fragPosLightSpace;
out vec3 v_surfaceNormal;
out vec3 v_toLightVector;
out vec2 v_texCoords;
out float v_shadowPower;

uniform mat4 u_modelViewProjectionMatrix; // Used for vertex transformation
uniform mat4 u_modelViewMatrix; // Used distance from camera
uniform mat4 u_modelMatrix; // Used for lighting calculations
uniform mat4 u_modelShadowMatrix;
uniform vec3 u_lightDirection;
uniform vec2 u_texOffset;
uniform float u_shadowDistance;
uniform int u_texRows;
uniform int u_useShadows;

const float shadowTransition = 10.0;

void main(void) {
	// Position computations
	vec4 position = vec4(a_position, 1.0);
	gl_Position = u_modelViewProjectionMatrix * position;

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
	if (u_useShadows == 1) {
		v_fragPosLightSpace = u_modelShadowMatrix * position;
		vec4 positionRelativeToCamera = u_modelViewMatrix * position;
		float distanceToCamera = length(positionRelativeToCamera.xyz);
		float distanceToShadowFail = distanceToCamera - (u_shadowDistance - shadowTransition);
		v_shadowPower = distanceToShadowFail / shadowTransition;
		v_shadowPower = clamp(1 - v_shadowPower, 0.0, 1.0);
	}
}