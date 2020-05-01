#pragma once

#include <glm/glm.hpp>

#include "Object.h"
#include "Shader.h"

namespace Joker {
	class ShadowShader : public Shader {
		// Renders models. No lighting, the only reason we have textures is for transparency
	public:
		ShadowShader(const char* vertex, const char* fragment);
		void render(Model& model, glm::mat4& modelMatrix);
		void calculateShadowMatrix(glm::vec3& lightDirection, glm::vec3& position);

		void uploadModelShadowMatrix(glm::mat4& ms);
		void uploadModelShadowMatrix(glm::mat4& m, glm::mat4& s);

		glm::mat4 shadowMatrix;
	private:
		GLint location_modelShadowMatrix;
	};
}