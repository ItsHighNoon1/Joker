#include "Joker.h"

namespace Joker {
	StaticShader::StaticShader(const char* vertex, const char* fragment) : ShaderProgram(vertex, fragment) {
		modelViewProjectionMatrix = glGetUniformLocation(programID, "u_modelViewProjectionMatrix");
		modelMatrix = glGetUniformLocation(programID, "u_modelMatrix");
		modelViewMatrix = glGetUniformLocation(programID, "u_modelViewMatrix");
		lightDirection = glGetUniformLocation(programID, "u_lightDirection");
		modelShadowMatrix = glGetUniformLocation(programID, "u_modelShadowMatrix");
		tex = glGetUniformLocation(programID, "u_tex");
		shadowMap = glGetUniformLocation(programID, "u_shadowMap");
		useShadows = glGetUniformLocation(programID, "u_useShadows");
		texRows = glGetUniformLocation(programID, "u_texRows");
		texOffset = glGetUniformLocation(programID, "u_texOffset");
		shadowDistance = glGetUniformLocation(programID, "u_shadowDistance");
		shadowMapSize = glGetUniformLocation(programID, "u_shadowMapSize");
	}
}