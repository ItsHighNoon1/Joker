#pragma once

#include <glm/glm.hpp>

#include "Model.h"
#include "Shader.h"

namespace Joker {
	class BasicShader : public Shader {
		// Basic shader should never be used, it's largely for development purposes
	public:
		BasicShader(const char* vertex, const char* fragment);
		void render(Model& model, glm::mat4& modelMatrix, glm::mat4& viewMatrix, glm::mat4& projectionMatrix);

		void uploadModelViewProjectionMatrix(glm::mat4& mvp);
		void uploadModelMatrix(glm::mat4& m);
		void uploadModelViewProjectionMatrix(glm::mat4& m, glm::mat4& v, glm::mat4& p);
		void uploadLightDirection(glm::vec3& dir);
	private:
		GLint location_modelViewProjectionMatrix;
		GLint location_modelMatrix;
		GLint location_lightDirection;
	};
}