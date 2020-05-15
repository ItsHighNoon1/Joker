#pragma once

#include "render/ShaderProgram.h"

namespace Joker {
	class ParticleShader : public ShaderProgram {
		// A shader that renders particles
	public:
		ParticleShader(const char* vertex, const char* fragment);
		int modelViewProjectionMatrix;
		int modelShadowMatrix;
		int tex;
		int shadowMap;
		int useShadows;
		int texOffset;
		int texRows;
	};
}