#pragma once

#include "render/ShaderProgram.h"

namespace Joker {
	class StaticShader : public ShaderProgram {
		// Basic shader should never be used, it's largely for development purposes
	public:
		StaticShader(const char* vertex, const char* fragment);
		int32_t modelViewProjectionMatrix;
		int32_t modelMatrix;
		int32_t modelViewMatrix;
		int32_t lightDirection;
		int32_t modelShadowMatrix;
		int32_t tex;
		int32_t shadowMap;
		int32_t useShadows;
		int32_t texOffset;
		int32_t texRows;
		int32_t shadowDistance;
		int32_t shadowMapSize;
	};
}