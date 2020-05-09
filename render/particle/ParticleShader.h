#pragma once

#include "../ShaderProgram.h"

namespace Joker {
	class ParticleShader : public ShaderProgram {
		// A shader that renders particles
	public:
		ParticleShader(const char* vertex, const char* fragment);
		int32_t modelViewProjectionMatrix;
		int32_t modelShadowMatrix;
		int32_t tex;
		int32_t shadowMap;
		int32_t useShadows;
		int32_t texOffset;
		int32_t texRows;
	};
}