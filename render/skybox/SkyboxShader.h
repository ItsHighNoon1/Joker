#pragma once

#include "render/ShaderProgram.h"

namespace Joker {
	class SkyboxShader : public ShaderProgram {
		// Renders the skybox
	public:
		SkyboxShader(const char* vertex, const char* fragment);
		int32_t viewProjectionMatrix;
	};
}