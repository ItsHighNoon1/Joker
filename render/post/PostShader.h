#pragma once

#include "render/ShaderProgram.h"

namespace Joker {
	class PostShader : public ShaderProgram {
		// The post shader is unique because the fragment shader does 100% of the work
	public:
		PostShader(const char* vertex, const char* fragment);
		int currentColor;
		int sceneColor;
		int sceneDepth;
	};
}