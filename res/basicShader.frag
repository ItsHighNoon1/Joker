#version 460 core

in vec4 v_fragPosLightSpace;
in vec3 v_surfaceNormal;
in vec3 v_toLightVector;
in vec2 v_texCoords;

out vec4 a_fragColor;

uniform sampler2D u_tex;
uniform sampler2D u_shadowMap;

void main(void) {
	// Sample the texture first, because if it's transparent we can skip the rest of the computations
	vec4 textureColor = texture(u_tex, v_texCoords);
	if (textureColor.a < 0.5) {
		discard;
	}

	// Normalize light vectors, not in vertex shader because the interpolation will mess it up
	vec3 unitNormal = normalize(v_surfaceNormal);
	vec3 unitLightVector = normalize(v_toLightVector);

	// Find the brightness of the pixel
	float brightness = dot(unitNormal, unitLightVector);

	// Check if the fragment is in shadow
	vec3 projCoords = v_fragPosLightSpace.xyz / v_fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(u_shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z - 0.01;
    float shadow = currentDepth > closestDepth ? 0.1 : 1.0;

	// Final color
	a_fragColor = vec4(shadow * brightness * textureColor.xyz, 1.0);
}