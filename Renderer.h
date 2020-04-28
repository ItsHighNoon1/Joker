#pragma once

#include "Shader.h"

namespace Joker {
	class Renderer {
		// Handles draw operations
	public:
		void prepare();
		void enableDepthTest();
		void disableDepthTest();
	};
}