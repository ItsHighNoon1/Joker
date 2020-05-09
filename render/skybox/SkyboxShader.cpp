#include "SkyboxShader.h"

#include "../ShaderProgram.h"

namespace Joker {
	SkyboxShader::SkyboxShader(const char* vertex, const char* fragment) : ShaderProgram(vertex, fragment) {
		viewProjectionMatrix = glGetUniformLocation(programID, "u_viewProjectionMatrix");
	}
}