#pragma once

#include <glm/glm.hpp>

#include "Object.h"
#include "Shader.h"

namespace Joker {
	class BasicShader : public Shader {
		// Basic shader should never be used, it's largely for development purposes
	public:
		BasicShader(const char* vertex, const char* fragment);
		void render(Model& model, glm::mat4& modelMatrix, glm::mat4& viewMatrix, glm::mat4& projectionMatrix, GLuint shadowTexture, glm::mat4& shadowMatrix);
		void render(Model& model, glm::mat4& modelMatrix, glm::mat4& viewMatrix, glm::mat4& projectionMatrix, GLuint shadowTexture, glm::mat4& shadowMatrix, uint32_t texIndex, uint32_t numRows);

		void uploadModelViewProjectionMatrix(glm::mat4& mvp);
		void uploadModelMatrix(glm::mat4& m);
		void uploadModelViewProjectionMatrix(glm::mat4& m, glm::mat4& v, glm::mat4& p);
		void uploadLightDirection(glm::vec3& dir);
		void uploadModelShadowMatrix(glm::mat4& ms);
		void uploadModelShadowMatrix(glm::mat4& m, glm::mat4& s);
		void uploadTextureAtlas(uint32_t index, uint32_t numRows);
	private:
		GLint location_modelViewProjectionMatrix;
		GLint location_modelMatrix;
		GLint location_lightDirection;
		GLint location_modelShadowMatrix;
		GLint location_tex;
		GLint location_shadowMap;
		GLint location_texOffset;
		GLint location_texRows;
	};
}