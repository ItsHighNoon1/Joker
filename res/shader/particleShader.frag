#version 330 core

in vec4 v_fragPosLightSpace;
in vec2 v_texCoords;

out vec4 a_fragColor;

uniform sampler2D u_tex;
uniform sampler2D u_shadowMap;
uniform int u_useShadows;

void main(void) {
	// Sample the texture first, because if it's transparent we can skip the rest of the computations
	vec4 textureColor = texture(u_tex, v_texCoords);
	if (textureColor.a < 0.5) {
		discard;
	}

	// Check if the fragment is in shadow
	float shadow = 1.0;
	if (u_useShadows == 1) {
		vec3 projCoords = v_fragPosLightSpace.xyz / v_fragPosLightSpace.w;
		projCoords = projCoords * 0.5 + 0.5;
		float closestDepth = texture(u_shadowMap, projCoords.xy).r; 
		float currentDepth = projCoords.z - 0.01;
		shadow = currentDepth > closestDepth ? 0.1 : 1.0;
	}

	// Final color
	a_fragColor = vec4(shadow * textureColor.xyz, 1.0);
}