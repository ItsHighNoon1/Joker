#include "BasicShader.h"

#include <glm/glm.hpp>

#include "Object.h"

namespace Joker {
	BasicShader::BasicShader(const char* vertex, const char* fragment) : Shader(vertex, fragment) {
		location_modelViewProjectionMatrix = glGetUniformLocation(programID, "u_modelViewProjectionMatrix");
		location_modelMatrix = glGetUniformLocation(programID, "u_modelMatrix");
		location_lightDirection = glGetUniformLocation(programID, "u_lightDirection");
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
		glEnableVertexAttribArray(2);

		// Draw command
		glDrawElements(GL_TRIANGLES, model.mesh.vertexCount, GL_UNSIGNED_INT, 0);

		// Disable buffers
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// Unbind model
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void BasicShader::uploadModelViewProjectionMatrix(glm::mat4& mvp) {
		uploadUniformMat4(location_modelViewProjectionMatrix, mvp);
	}

	void BasicShader::uploadModelMatrix(glm::mat4& m) {
		uploadUniformMat4(location_modelMatrix, m);
	}

	void BasicShader::uploadModelViewProjectionMatrix(glm::mat4& m, glm::mat4& v, glm::mat4& p) {
		glm::mat4 modelViewMatrix = v * m;
		glm::mat4 modelViewProjectionMatrix = p * modelViewMatrix;
		uploadModelMatrix(m);
		uploadModelViewProjectionMatrix(modelViewProjectionMatrix);
	}

	void BasicShader::uploadLightDirection(glm::vec3& dir) {
		uploadUniformVec3(location_lightDirection, dir);
	}
}