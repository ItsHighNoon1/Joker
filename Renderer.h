#pragma once

#include "Object.h"

namespace Joker {
	class Renderer {
		// Handles draw operations
	public:
		void prepare();
		void enableDepthTest();
		void disableDepthTest();
		void bindFrameBuffer(Framebuffer& buffer);
		void bindDefaultFrameBuffer();
	};
}