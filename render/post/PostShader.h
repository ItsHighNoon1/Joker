#pragma once

#include "../ShaderProgram.h"

namespace Joker {
	class PostShader : public ShaderProgram {
		// The post shader is unique because the fragment shader does 100% of the work
	public:
		PostShader(const char* vertex, const char* fragment);
		int32_t currentColor;
		int32_t sceneColor;
		int32_t sceneDepth;
	};
}