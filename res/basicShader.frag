#version 330 core

in vec4 v_fragPosLightSpace;
in vec3 v_surfaceNormal;
in vec3 v_toLightVector;
in vec2 v_texCoords;
in float v_shadowPower;

out vec4 a_fragColor;

uniform sampler2D u_tex;
uniform sampler2D u_shadowMap;
uniform int u_shadowMapSize;

const int pcfCount = 5;
const float totalTexels = (pcfCount * 2.0 + 1.0) * (pcfCount * 2.0 + 1.0);

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
	float diffuse = dot(unitNormal, unitLightVector);

	// Smooth shadows using PCF
	float shadow = 1.0;
	if (v_shadowPower > 0) {
		vec3 projCoords = v_fragPosLightSpace.xyz / v_fragPosLightSpace.w;
		projCoords = projCoords * 0.5 + 0.5;
		float texelSize = 1.0 / u_shadowMapSize;
		float total = 0.0;
		for (int x = -pcfCount; x <= pcfCount; x++) {
			for (int y = -pcfCount; y <= pcfCount; y++) {
				float closestDepth = texture(u_shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
				float currentDepth = projCoords.z - 0.01;
				if (currentDepth > closestDepth) {
					total += 1.0;
				}
			}
		}
		shadow -= v_shadowPower * total / totalTexels;
	}

	float brightness = shadow * diffuse;
	brightness = max(brightness, 0.1);

	// Final color
	a_fragColor = vec4(brightness * textureColor.xyz, 1.0);
}