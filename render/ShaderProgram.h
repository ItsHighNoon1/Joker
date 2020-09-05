#pragma once

#include <glad/glad.h>

namespace Joker {
	class ShaderProgram {
		// Represents a shader, notice that there are no uniforms, this is covered by children of this class
	public:
		ShaderProgram(const char* vShader, const char* fShader);
		void deleteShader();
		int programID;
	};
}