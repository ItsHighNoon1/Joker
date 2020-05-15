#include "InputHandler.h"

namespace Joker {
	// https://www.glfw.org/docs/3.3/group__input.html

	InputHandler::InputHandler() {}

	InputHandler::InputHandler(GLFWwindow* w, uint32_t* windowWidth, uint32_t* windowHeight) {
		// Need these for querying inputs
		window = w;
		width = windowWidth;
		height = windowHeight;

		// Ideally we use raw mouse motion, but sometimes it isn't supported
		if (glfwRawMouseMotionSupported()) {
			glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		}
	}

	void InputHandler::registerKeyCallback(GLFWkeyfun callback) {
		glfwSetKeyCallback(window, callback);
	}

	void InputHandler::registerMouseButtonCallback(GLFWmousebuttonfun callback) {
		glfwSetMouseButtonCallback(window, callback);
	}

	void InputHandler::registerMouseScrollCallback(GLFWscrollfun callback) {
		glfwSetScrollCallback(window, callback);
	}

	int InputHandler::getKeyState(int key) {
		return glfwGetKey(window, key);
	}

	int InputHandler::getMouseButtonState(int btn) {
		return glfwGetMouseButton(window, btn);
	}

	void InputHandler::getMousePosition(float& xpos, float& ypos) {
		// For things like clicking on menus, which need screenspace coordinates
		double doubleXpos;
		double doubleYpos;
		glfwGetCursorPos(window, &doubleXpos, &doubleYpos);
		xpos = 2 * ((float)doubleXpos / *width) - 1.0f;
		ypos = 2 * ((float)-doubleYpos / *height) + 1.0f;
	}

	void InputHandler::getRawMousePosition(float& xpos, float& ypos) {
		// For things like 3D cameras, which rely on raw mouse movements
		double doubleXpos;
		double doubleYpos;
		glfwGetCursorPos(window, &doubleXpos, &doubleYpos);
		xpos = (float)doubleXpos;
		ypos = (float)-doubleYpos; // GLFW measures distance from top, but up should be greater so we flip it
	}

	void InputHandler::setInputMode(int mode, int value) {
		// Check docs on this one
		glfwSetInputMode(window, mode, value);
	}
}