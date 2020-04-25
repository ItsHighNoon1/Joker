#include "BasicShader.h"

#include <iostream>

#include <glm/glm.hpp>

#include "Model.h"

namespace Joker {
	BasicShader::BasicShader(const char* vertex, const char* fragment) : Shader(vertex, fragment) {
		location_modelViewProjection = glGetUniformLocation(programID, "modelViewProjectionMatrix");
		std::cout << location_modelViewProjection << std::endl;
	}

	void BasicShader::render(Mesh& mesh, glm::mat4& modelMatrix, glm::mat4& viewMatrix, glm::mat4& projectionMatrix) {
		uploadModelViewProjectionMatrix(modelMatrix, viewMatrix, projectionMatrix);
		glBindVertexArray(mesh.vaoID); // Bind
		glEnableVertexAttribArray(0); // Enable position buffer
		glDrawElements(GL_TRIANGLES, mesh.vertexCount, GL_UNSIGNED_INT, 0); // Draw
		glDisableVertexAttribArray(0); // Disable position buffer
		glBindVertexArray(0); // Unbind
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