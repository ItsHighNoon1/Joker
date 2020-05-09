#include "PostShader.h"

#include "../ShaderProgram.h"

namespace Joker {
	PostShader::PostShader(const char* vertex, const char* fragment) : ShaderProgram(vertex, fragment) {
		currentColor = glGetUniformLocation(programID, "u_currentColor");
		sceneColor = glGetUniformLocation(programID, "u_sceneColor");
		sceneDepth = glGetUniformLocation(programID, "u_depthColor");
	}
}