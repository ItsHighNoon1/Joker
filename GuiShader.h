#pragma once

#include <glm/glm.hpp>

#include "Object.h"
#include "Shader.h"

namespace Joker {
	class GuiShader : public Shader {
		// GUI shader, just renders a quad so not very complex
	public:
		GuiShader(const char* vertex, const char* fragment);
		void render(Model& gui, glm::mat4& transformationMatrix);

		void uploadTransformationMatrix(glm::mat4& m);
	private:
		GLint location_transformationMatrix;
	};
}