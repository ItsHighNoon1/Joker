#include "BasicShader.h"

#include <iostream>

#include <glm/glm.hpp>

namespace Joker {
	BasicShader::BasicShader(const char* vertex, const char* fragment) : Shader(vertex, fragment) {
		location_modelViewProjection = glGetUniformLocation(programID, "modelViewProjectionMatrix");
		std::cout << location_modelViewProjection << std::endl;
	}

	void BasicShader::uploadModelViewProjectionMatrix(glm::mat4& mvp) {
		uploadUniformMat4(location_modelViewProjection, mvp);
	}

	void BasicShader::uploadModelViewProjectionMatrix(glm::mat4& m, glm::mat4& v, glm::mat4& p) {
		glm::mat4 modelViewMatrix = v * m;
		glm::mat4 modelViewProjectionMatrix = p * modelViewMatrix;
		uploadModelViewProjectionMatrix(modelViewProjectionMatrix);
	}
}