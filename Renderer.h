#pragma once

#include "Model.h"
#include "Shader.h"

namespace Joker {
	class Renderer {
		// Handles draw operations
	public:
		void prepare();
		void render(Shader shader, Mesh mesh);
	};
}