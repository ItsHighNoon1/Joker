#pragma once

#include "render/ShaderProgram.h"

namespace Joker {
	class ShadowShader : public ShaderProgram {
		// Renders models. No lighting, the only reason we have textures is for transparency
	public:
		ShadowShader(const char* vertex, const char* fragment);
		int32_t modelShadowMatrix;
		int32_t texOffset;
		int32_t texRows;
	};
}