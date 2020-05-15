#pragma once

#include "render/ShaderProgram.h"

namespace Joker {
	class TextShader : public ShaderProgram {
		// GUI shader, just renders a quad so not very complex
	public:
		TextShader(const char* vertex, const char* fragment);
		int transformationMatrix;
		int charPosition;
		int charSize;
		int charOffset;
		int textColor;
	};
}