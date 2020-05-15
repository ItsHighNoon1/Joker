#pragma once

#include "render/ShaderProgram.h"

namespace Joker {
	class StaticShader : public ShaderProgram {
		// Basic shader should never be used, it's largely for development purposes
	public:
		StaticShader(const char* vertex, const char* fragment);
		int modelViewProjectionMatrix;
		int modelMatrix;
		int modelViewMatrix;
		int lightDirection;
		int modelShadowMatrix;
		int tex;
		int shadowMap;
		int useShadows;
		int texOffset;
		int texRows;
		int shadowDistance;
		int shadowMapSize;
	};
}