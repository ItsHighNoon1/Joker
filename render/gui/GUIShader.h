#pragma once

#include "render/ShaderProgram.h"

namespace Joker {
	class GUIShader : public ShaderProgram {
		// GUI shader, just renders a quad so not very complex
	public:
		GUIShader(const char* vertex, const char* fragment);
		int transformationMatrix;
		int texOffset;
		int texRows;
	};
}