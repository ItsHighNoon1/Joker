#version 330 core

in vec2 v_texCoords;

out vec4 a_fragColor;

uniform sampler2D u_currentColor;

const float crtSize = 0.01;
const float crtThird = crtSize / 3.0;

void main(void) {
	vec4 colorMask = vec4(0.0, 0.0, 0.0, 1.0);
	float crtValue = mod(v_texCoords.x, crtSize);
	crtValue = mod(crtValue, crtSize);
	if (crtValue < crtThird) {
		colorMask.x = 1.0;
	} else if (crtValue < crtThird * 2) {
		colorMask.y = 1.0;
	} else {
		colorMask.z = 1.0;
	}
	
	a_fragColor = colorMask * texture(u_currentColor, v_texCoords);
}