#include "PostShader.h"

#include "../ShaderProgram.h"

namespace Joker {
	PostShader::PostShader(const char* vertex, const char* fragment) : ShaderProgram(vertex, fragment) {
		modelShadowMatrix = glGetUniformLocation(programID, "u_modelShadowMatrix");
		texRows = glGetUniformLocation(programID, "u_texRows");
		texOffset = glGetUniformLocation(programID, "u_texOffset");
	}
}