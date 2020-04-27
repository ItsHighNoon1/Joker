#version 400 core

in vec3 v_surfaceNormal;
in vec3 v_toLightVector;
in vec2 v_texCoords;

out vec4 a_fragColor;

uniform sampler2D u_tex;

void main(void) {
	// Normalize light vectors, not in vertex shader because the interpolation will mess it up
	vec3 unitNormal = normalize(v_surfaceNormal);
	vec3 unitLightVector = normalize(v_toLightVector);

	// Find the brightness of the pixel
	float brightness = dot(unitNormal, unitLightVector);

	// Final color
	a_fragColor = brightness * texture(u_tex, v_texCoords);
}