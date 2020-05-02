#include "BasicShader.h"

#include <glm/glm.hpp>

#include "Shader.h"
#include "Object.h"
#include "Log.h"

namespace Joker {
	BasicShader::BasicShader(const char* vertex, const char* fragment) : Shader(vertex, fragment) {
		location_modelViewProjectionMatrix = glGetUniformLocation(programID, "u_modelViewProjectionMatrix");
		location_modelMatrix = glGetUniformLocation(programID, "u_modelMatrix");
		location_lightDirection = glGetUniformLocation(programID, "u_lightDirection");
		location_modelShadowMatrix = glGetUniformLocation(programID, "u_modelShadowMatrix");
		location_tex = glGetUniformLocation(programID, "u_tex");
		location_shadowMap = glGetUniformLocation(programID, "u_shadowMap");
		location_texRows = glGetUniformLocation(programID, "u_texRows");
		location_texOffset = glGetUniformLocation(programID, "u_texOffset");
	}

	void BasicShader::render(Model& model, glm::mat4& modelMatrix, glm::mat4& viewMatrix, glm::mat4& projectionMatrix, GLuint shadowTexture, glm::mat4& shadowMatrix) {
		// Bind model
		glBindVertexArray(model.mesh.vaoID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, model.texture);

		// Bind shadow map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, shadowTexture);

		// Upload uniforms
		uploadModelViewProjectionMatrix(modelMatrix, viewMatrix, projectionMatrix);
		uploadModelShadowMatrix(modelMatrix, shadowMatrix);
		uploadUniformInt(location_tex, 0);
		uploadUniformInt(location_shadowMap, 1);

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
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void BasicShader::render(Model& model, glm::mat4& modelMatrix, glm::mat4& viewMatrix, glm::mat4& projectionMatrix, GLuint shadowTexture, glm::mat4& shadowMatrix, uint32_t texIndex, uint32_t numRows) {
		// Upload the texture atlas uniforms
		uploadTextureAtlas(texIndex, numRows);

		// Normal render
		render(model, modelMatrix, viewMatrix, projectionMatrix, shadowTexture, shadowMatrix);

		// Reset the texture atlas uniforms
		uploadUniformInt(location_texRows, 0);
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

	void BasicShader::uploadModelShadowMatrix(glm::mat4& ms) {
		uploadUniformMat4(location_modelShadowMatrix, ms);
	}

	void BasicShader::uploadModelShadowMatrix(glm::mat4& m, glm::mat4& s) {
		glm::mat4 modelShadowMatrix = s * m;
		uploadModelShadowMatrix(modelShadowMatrix);
	}

	void BasicShader::uploadTextureAtlas(uint32_t index, uint32_t numRows) {
		glm::vec2 offset = glm::vec2((float)(index % numRows) / numRows, (float)(index / numRows) / numRows);
		uploadUniformInt(location_texRows, numRows);
		uploadUniformVec2(location_texOffset, offset);
	}
}