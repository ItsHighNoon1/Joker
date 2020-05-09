#include "ShadowShader.h"

#include "Joker.h"

namespace Joker {
	ShadowShader::ShadowShader(const char* vertex, const char* fragment) : ShaderProgram(vertex, fragment) {
		modelShadowMatrix = glGetUniformLocation(programID, "u_modelShadowMatrix");
		texRows = glGetUniformLocation(programID, "u_texRows");
		texOffset = glGetUniformLocation(programID, "u_texOffset");
	}
}