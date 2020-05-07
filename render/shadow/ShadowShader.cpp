#include "ShadowShader.h"

#include "../ShaderProgram.h"

namespace Joker {
	ShadowShader::ShadowShader(const char* vertex, const char* fragment) : ShaderProgram(vertex, fragment) {
		modelShadowMatrix = glGetUniformLocation(programID, "u_modelShadowMatrix");
	}
}