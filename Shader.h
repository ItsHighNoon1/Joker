#pragma once

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Joker {
	class Shader {
		// Represents a shader, notice that there are no uniforms, this is covered by children of this class
	public:
		Shader(const char* vShader, const char* fShader);
		void start();
		void stop();
		void cleanUp();
	protected: // Visible to subclasses
		GLuint programID;

		// These don't depend on the instance of shader, but they shouldn't be visible outside of a shader so we keep them in the class
		void uploadUniformInt(GLint location, int i);
		void uploadUniformFloat(GLint location, float f);
		void uploadUniformVec2(GLint location, glm::vec2& v);
		void uploadUniformVec3(GLint location, glm::vec3& v);
		void uploadUniformVec4(GLint location, glm::vec4& v);
		void uploadUniformMat4(GLint location, glm::mat4& m);
	};
}