#pragma once

#include "Model.h"

namespace Joker {
	class Renderer {
		// Handles draw operations
	public:
		void prepare();
		void render(Mesh m);
	};
}