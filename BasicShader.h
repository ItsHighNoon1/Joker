#pragma once

#include <glm/glm.hpp>

#include "Shader.h"

namespace Joker {
	class BasicShader : public Shader {
		// Basic shader should never be used, it's largely for development purposes
	public:
		BasicShader(const char* vertex, const char* fragment);
		void uploadModelViewProjectionMatrix(glm::mat4& mvp);
		void uploadModelViewProjectionMatrix(glm::mat4& m, glm::mat4& v, glm::mat4& p);
	private:
		GLint location_modelViewProjection;
	};
}