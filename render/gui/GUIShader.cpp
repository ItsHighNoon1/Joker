#include "GUIShader.h"

#include "../ShaderProgram.h"

namespace Joker {
	GUIShader::GUIShader(const char* vertex, const char* fragment) : ShaderProgram(vertex, fragment) {
		transformationMatrix = glGetUniformLocation(programID, "u_transformationMatrix");
	}
}