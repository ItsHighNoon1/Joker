#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "io/InputHandler.h"

namespace Joker {
	class DisplayManager {
		// Handle for all things GLFW
	public:
		DisplayManager();
		~DisplayManager();
		void updateDisplay();
		bool shouldClose();

		InputHandler input;
		float dt = 0;

		static uint32_t windowWidth;
		static uint32_t windowHeight;
	private:
		GLFWwindow* window = nullptr;
		float lastMouseX = 0;
		float lastMouseY = 0;
		float lastTime = 0;
	};
}