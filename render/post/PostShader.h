#pragma once

#include "../ShaderProgram.h"

namespace Joker {
	class PostShader : public ShaderProgram {
		// Renders models. No lighting, the only reason we have textures is for transparency
	public:
		PostShader(const char* vertex, const char* fragment);
		int32_t modelShadowMatrix;
		int32_t texOffset;
		int32_t texRows;
	};
}