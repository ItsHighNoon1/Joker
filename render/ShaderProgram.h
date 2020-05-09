#pragma once

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Joker {
	class ShaderProgram {
		// Represents a shader, notice that there are no uniforms, this is covered by children of this class
	public:
		ShaderProgram(const char* vShader, const char* fShader);
		~ShaderProgram();
		GLuint programID;
	};
}