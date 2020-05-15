#include "ParticleShader.h"

namespace Joker {
	ParticleShader::ParticleShader(const char* vertex, const char* fragment) : ShaderProgram(vertex, fragment) {
		modelViewProjectionMatrix = glGetUniformLocation(programID, "u_modelViewProjectionMatrix");
		modelShadowMatrix = glGetUniformLocation(programID, "u_modelShadowMatrix");
		tex = glGetUniformLocation(programID, "u_tex");
		shadowMap = glGetUniformLocation(programID, "u_shadowMap");
		useShadows = glGetUniformLocation(programID, "u_useShadows");
		texRows = glGetUniformLocation(programID, "u_texRows");
		texOffset = glGetUniformLocation(programID, "u_texOffset");
	}
}