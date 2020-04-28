#include "Shader.h"

#include <fstream>
#include <sstream>
#include <string> 
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Log.h"

namespace Joker {
	GLuint loadShader(const char* path, GLuint type) {
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
			JK_CORE_WARN("Failed to open file ({0})", path);
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
			JK_CORE_WARN("Shader compilation error", shaderErrorMessage.data());
		}

		return shader;
	}

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
			JK_CORE_ERROR("Program linking error", programError.data());
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

	void Shader::uploadUniformInt(GLint location, int i) {
		// Note that this will always upload to the current shader
		glUniform1i(location, i);
	}

	void Shader::uploadUniformFloat(GLint location, float f) {
		glUniform1f(location, f);
	}

	void Shader::uploadUniformVec2(GLint location, glm::vec2& v) {
		glUniform2f(location, v.x, v.y);
	}

	void Shader::uploadUniformVec3(GLint location, glm::vec3& v) {
		glUniform3f(location, v.x, v.y, v.z);
	}

	void Shader::uploadUniformVec4(GLint location, glm::vec4& v) {
		glUniform4f(location, v.x, v.y, v.z, v.w);
	}

	void Shader::uploadUniformMat4(GLint location, glm::mat4& m) {
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m));
	}
}