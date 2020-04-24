#pragma once

#include <glad/glad.h>

namespace Joker {
	class Shader {
		// Represents a shader, this will probably need expansion with uniforms
	public:
		Shader(const char* vShader, const char* fShader);
		void start();
		void stop();
		void cleanUp();
	private:
		GLuint programID;

		GLuint loadShader(const char* path, GLuint type);
	};
}