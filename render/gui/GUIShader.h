#pragma once

#include "render/ShaderProgram.h"

namespace Joker {
	class GUIShader : public ShaderProgram {
		// GUI shader, just renders a quad so not very complex
	public:
		GUIShader(const char* vertex, const char* fragment);
		int32_t transformationMatrix;
		int32_t texOffset;
		int32_t texRows;
	};
}