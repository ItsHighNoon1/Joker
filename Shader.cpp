#include "Shader.h"

#include <fstream>
#include <sstream>
#include <string> 
#include <vector>

#include <glad/glad.h>

namespace Joker {
	Shader::Shader(const char* vShader, const char* fShader) {
		// Load the two shaders
		GLuint vertexShader = loadShader(vShader, GL_VERTEX_SHADER);
		GLuint fragmentShader = loadShader(fShader, GL_FRAGMENT_SHADER);

		// Create the shader program
		programID = glCreateProgram();
		glAttachShader(programID, vertexShader);
		glAttachShader(programID, fragmentShader);
		glLinkProgram(programID);

		// Check the program
		GLint result = GL_FALSE;
		GLint infoLogLength;
		glGetProgramiv(programID, GL_LINK_STATUS, &result);
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0) {
			std::vector<char> programError(infoLogLength + 1);
			glGetProgramInfoLog(programID, infoLogLength, NULL, &programError[0]);
			printf("%s\n", &programError[0]);
		}

		// The program is created, we have no use for the individual shaders
		glDetachShader(programID, vertexShader);
		glDetachShader(programID, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	void Shader::start() {
		glUseProgram(programID);
	}

	void Shader::stop() {
		glUseProgram(0);
	}

	void Shader::cleanUp() {
		glDeleteProgram(programID);
	}

	GLuint Shader::loadShader(const char* path, GLuint type) {
		GLuint shader = glCreateShader(type); // Create

		// Read the file and get the shader code
		std::string source;
		std::ifstream sourceStream(path, std::ios::in);
		if (sourceStream.is_open()) {
			std::stringstream sstr;
			sstr << sourceStream.rdbuf();
			source = sstr.str();
			sourceStream.close();
		} else {
			printf("Couldn't find file: %s\n", path);
			return 0;
		}

		// Compile shader
		char const* sourcePointer = source.c_str();
		glShaderSource(shader, 1, &sourcePointer, NULL);
		glCompileShader(shader);

		// Check for problems
		GLint result = GL_FALSE;
		GLint infoLogLength;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0) {
			std::vector<char> shaderErrorMessage(infoLogLength + 1);
			glGetShaderInfoLog(shader, infoLogLength, NULL, &shaderErrorMessage[0]);
			printf("%s\n", &shaderErrorMessage[0]);
		}

		return shader;
	}
}