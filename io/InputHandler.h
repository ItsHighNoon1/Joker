#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Joker {
	class InputHandler {
		// Light wrapper around all of GLFW's input functions
	public:
		InputHandler(); // TODO hack
		InputHandler(GLFWwindow* w, uint32_t* windowWidth, uint32_t* windowHeight);

		void registerKeyCallback(GLFWkeyfun callback);
		void registerMouseButtonCallback(GLFWmousebuttonfun callback);
		void registerMouseScrollCallback(GLFWscrollfun callback);
		int getKeyState(int key);
		int getMouseButtonState(int btn);
		void getMousePosition(float& xpos, float& ypos);
		void getRawMousePosition(float& xpos, float& ypos);
		void setInputMode(int mode, int value);

		// DisplayManager calculates these, but they really belong here
		float dx = 0.0f;
		float dy = 0.0f;
	private:
		GLFWwindow* window;
		uint32_t* width;
		uint32_t* height;
	};
}