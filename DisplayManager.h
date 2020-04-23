#pragma once

#include <glad/glad.h> // We don't use glad in this file, but GLFW will fuck up if we don't include it first
#include <GLFW/glfw3.h>

namespace Joker {
	class DisplayManager {
		// Handle for all things GLFW
	public:
		DisplayManager();
		~DisplayManager();
		void updateDisplay();
		bool shouldClose();
	private:
		GLFWwindow* window;

		static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
	};
}