#include "StaticShader.h"

#include "../ShaderProgram.h"

namespace Joker {
	StaticShader::StaticShader(const char* vertex, const char* fragment) : ShaderProgram(vertex, fragment) {
		modelViewProjectionMatrix = glGetUniformLocation(programID, "u_modelViewProjectionMatrix");
		modelMatrix = glGetUniformLocation(programID, "u_modelMatrix");
		lightDirection = glGetUniformLocation(programID, "u_lightDirection");
		modelShadowMatrix = glGetUniformLocation(programID, "u_modelShadowMatrix");
		tex = glGetUniformLocation(programID, "u_tex");
		shadowMap = glGetUniformLocation(programID, "u_shadowMap");
		texRows = glGetUniformLocation(programID, "u_texRows");
		texOffset = glGetUniformLocation(programID, "u_texOffset");
	}

	void uploadTextureAtlas(uint32_t index, uint32_t numRows) {
		
	}
}