#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Joker {
	struct Mesh {
		// Everything* OpenGL uses is 32-bit, previously we needed to use GL types to ensure that we met that
		// but C++11 gave us fixed width int types which we will be using to keep the code cleaner
		uint32_t vaoID;
		uint32_t vertexCount;
	};

	class APIWrapper {
	public:
		APIWrapper();
		~APIWrapper();

		Mesh loadToVAO();
		void poll();
		bool shouldClose();
	private:
		static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
		uint32_t createVAO();

		GLFWwindow* window;
	};
}