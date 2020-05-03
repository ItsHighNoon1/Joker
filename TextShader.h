#pragma once

#include <glm/glm.hpp>

#include "Object.h"
#include "Shader.h"

namespace Joker {
	class TextShader : public Shader {
		// GUI shader, just renders a quad so not very complex
	public:
		TextShader(const char* vertex, const char* fragment);
		void render(Text& text, glm::mat4& transformationMatrix, Model& texModel);

		void uploadTransformationMatrix(glm::mat4& m);
		void uploadCharPosition(glm::vec2& pos);
		void uploadCharSize(glm::vec2& size);
		void uploadCharOffset(glm::vec2& off);
	private:
		GLint location_transformationMatrix;
		GLint location_charPosition;
		GLint location_charSize;
		GLint location_charOffset;
	};
}