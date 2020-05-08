#pragma once

#include <glad/glad.h> // We don't use glad in this file, but GLFW will fuck up if we don't include it first
#include <GLFW/glfw3.h>

#include "InputHandler.h"

namespace Joker {
	class DisplayManager {
		// Handle for all things GLFW
	public:
		DisplayManager();
		~DisplayManager();
		void updateDisplay();
		bool shouldClose();

		InputHandler input;
		float dt;

		static uint32_t windowWidth;
		static uint32_t windowHeight;
	private:
		GLFWwindow* window;
		float lastMouseX;
		float lastMouseY;
		float lastTime;
	};
}