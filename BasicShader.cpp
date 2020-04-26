#include "BasicShader.h"

#include <iostream>

#include <glm/glm.hpp>

#include "Model.h"

namespace Joker {
	BasicShader::BasicShader(const char* vertex, const char* fragment) : Shader(vertex, fragment) {
		location_modelViewProjection = glGetUniformLocation(programID, "modelViewProjectionMatrix");
		std::cout << location_modelViewProjection << std::endl;
	}

	void BasicShader::render(Model& model, glm::mat4& modelMatrix, glm::mat4& viewMatrix, glm::mat4& projectionMatrix) {
		// Bind model
		glBindVertexArray(model.mesh.vaoID);
		glBindTexture(GL_TEXTURE_2D, model.texture);

		// Upload uniforms
		uploadModelViewProjectionMatrix(modelMatrix, viewMatrix, projectionMatrix);

		// Enable buffers
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		// Draw command
		glDrawElements(GL_TRIANGLES, model.mesh.vertexCount, GL_UNSIGNED_INT, 0);

		// Disable buffers
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Unbind model
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
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