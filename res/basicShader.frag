#version 400 core

in vec2 v_texCoords;

out vec4 fragColor;

uniform sampler2D u_tex;

void main(void) {
	fragColor = texture(u_tex, v_texCoords); // Use the color of the texture
}