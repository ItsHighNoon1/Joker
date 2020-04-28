#pragma once

#include <time.h>

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
	private:
		GLFWwindow* window;
		float lastMouseX;
		float lastMouseY;
		clock_t lastTime;
	};
}