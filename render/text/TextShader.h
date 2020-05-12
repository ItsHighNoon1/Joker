#pragma once

#include "render/ShaderProgram.h"

namespace Joker {
	class TextShader : public ShaderProgram {
		// GUI shader, just renders a quad so not very complex
	public:
		TextShader(const char* vertex, const char* fragment);
		int32_t transformationMatrix;
		int32_t charPosition;
		int32_t charSize;
		int32_t charOffset;
		int32_t textColor;
	};
}