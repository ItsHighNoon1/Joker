#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Joker {
	class InputHandler {
		// Light wrapper around all of GLFW's input functions
	public:
		InputHandler(); // TODO hack
		InputHandler(GLFWwindow* w, int* windowWidth, int* windowHeight);

		void registerKeyCallback(GLFWkeyfun callback);
		void registerMouseButtonCallback(GLFWmousebuttonfun callback);
		void registerMouseScrollCallback(GLFWscrollfun callback);
		int getKeyState(int key);
		int getMouseButtonState(int btn);
		void getMousePosition(float& xpos, float& ypos);
		void getRawMousePosition(float& xpos, float& ypos);
		void setInputMode(int mode, int value);
		void updateMouse();
		float getDx();
		float getDy();
	private:
		GLFWwindow* window;
		int* width;
		int* height;
		float dx = 0.0f;
		float dy = 0.0f;
		float lastMouseX = 0.0f;
		float lastMouseY = 0.0f;
	};
}