#include "ShadowShader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Object.h"

namespace Joker {
	ShadowShader::ShadowShader(const char* vertex, const char* fragment) : Shader(vertex, fragment) {
		location_modelShadowMatrix = glGetUniformLocation(programID, "u_modelShadowMatrix");
	}

	void ShadowShader::render(Model& model, glm::mat4& modelMatrix) {
		// Bind model
		glBindVertexArray(model.mesh.vaoID);
		glBindTexture(GL_TEXTURE_2D, model.texture);

		// Upload uniforms
		uploadModelShadowMatrix(modelMatrix, shadowMatrix);

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

	void ShadowShader::calculateShadowMatrix(glm::vec3& lightDirection, glm::vec3& position) {
		// Calculate our shadow matrix
		glm::mat4 lightProjection = glm::ortho(-500.0f, 500.0f, -500.0f, 500.0f, 1.0f, 2000.0f); // Orthographic projection matrix
		glm::mat4 lightView = glm::lookAt(75.0f * glm::normalize(-lightDirection) + position, position, glm::vec3(0.0f, 1.0f, 0.0f)); // Look at something
		shadowMatrix = lightProjection * lightView;
	}

	void ShadowShader::uploadModelShadowMatrix(glm::mat4& ms) {
		uploadUniformMat4(location_modelShadowMatrix, ms);
	}

	void ShadowShader::uploadModelShadowMatrix(glm::mat4& m, glm::mat4& s) {
		glm::mat4 modelShadowMatrix = s * m;
		uploadModelShadowMatrix(modelShadowMatrix);
	}
}