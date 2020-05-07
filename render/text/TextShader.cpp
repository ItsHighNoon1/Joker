#include "TextShader.h"

#include "../ShaderProgram.h"

namespace Joker {
	TextShader::TextShader(const char* vertex, const char* fragment) : ShaderProgram(vertex, fragment) {
		transformationMatrix = glGetUniformLocation(programID, "u_transformationMatrix");
		charPosition = glGetUniformLocation(programID, "u_charPosition");
		charSize = glGetUniformLocation(programID, "u_charSize");
		charOffset = glGetUniformLocation(programID, "u_charOffset");
		textColor = glGetUniformLocation(programID, "u_textColor");
	}
}