#include "DisplayManager.h"

#include <chrono>
#include <iostream>

namespace Joker {
	DisplayManager::DisplayManager(int width, int height, const char* name) {
		// Initialize GLFW
		glfwInit();

		// Specify OpenGL version
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Create window and check if it worked
		window = glfwCreateWindow(width, height, name, nullptr, nullptr);
		if (window == nullptr) {
			glfwTerminate();
		}
		glfwMakeContextCurrent(window);

		// We will use this to call the resize method as a callback
		glfwSetWindowUserPointer(window, this);
		glfwSetWindowSizeCallback(window, DisplayManager::resizeCallback);

		// Initialize glad, if this doesn't work we have no game and glViewport() will force a crash anyways
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		// Resize callback doesn't get called on creation, so we just force an update here
		updateViewport();

		// Get an input handler
		inputHandler = InputHandler(window, &width, &height);
	}

	DisplayManager::~DisplayManager() {
		// Clean up GLFW
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	bool DisplayManager::update() {
		// Speed
		glfwSwapInterval(0);

		// Update the screen
		glfwSwapBuffers(window);
		glfwPollEvents();

		// Calculate frame time
		long long int currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		int deltaNs = currentTime - lastTime;
		lastTime = currentTime;
		frameTime = deltaNs * 0.000000001f;

		// If the frame time was excessively long, it's probably a better idea to pretend it was 0
		if (frameTime > 0.1f) {
			frameTime = 0.0f;
		}

		// Recalculate mouse movement
		inputHandler.updateMouse();

		// Return window status
		return !glfwWindowShouldClose(window);
	}

	bool DisplayManager::getKey(int key) {
		// Wrapper
		return glfwGetKey(window, key) == GLFW_PRESS;
	}

	bool DisplayManager::getMouseButton(int button) {
		// Wrapper
		return glfwGetMouseButton(window, button) == GLFW_PRESS;
	}

	void DisplayManager::getMousePos(float* x, float* y) {
		// Get mouse pos, then turn it into screen space coordinates
		double mx;
		double my;
		glfwGetCursorPos(window, &mx, &my);
		mx /= width;
		my /= height;
		*x = (float)mx * 2.0f - 1.0f;
		*y = (float)my * -2.0f + 1.0f;
	}

	void DisplayManager::fullscreen(bool fullscreen) {
		// Changing window monitor counts as a resize, so it will run our resize callback
		if (fullscreen) {
			// Set fullscreen
			if (glfwGetWindowMonitor(window) != nullptr) {
				// The window is already fullscreen, don't waste time
				return;
			}

			// Store the location and size of the window so we can find the best monitor or revert to windowed
			glfwGetWindowPos(window, &windowedX, &windowedY);
			glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

			// Set fullscreen, we need the video mode and the monitor to go fullscreen on
			GLFWmonitor* monitor = bestMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		} else {
			// Set windowed
			if (glfwGetWindowMonitor(window) == nullptr) {
				// The window is already windowed, don't waste time
				return;
			}

			// Just set it back to windowed mode, simple
			glfwSetWindowMonitor(window, nullptr, windowedX, windowedY, windowedWidth, windowedHeight, GLFW_DONT_CARE);
		}
	}

	float DisplayManager::getLastDeltaTime() {
		// Getter for frameTime
		return frameTime;
	}

	int DisplayManager::getWidth() {
		// Getter for width
		return width;
	}

	int DisplayManager::getHeight() {
		// Getter for height
		return height;
	}

	bool DisplayManager::getFullscreen() {
		// Getter for fullscreen status
		return glfwGetWindowMonitor(window) != nullptr;
	}

	InputHandler& DisplayManager::getInputHandler() {
		// Getter for input handler
		return inputHandler;
	}

	void DisplayManager::resizeCallback(GLFWwindow* window, int width, int height) {
		// Send this event to the class responsible for this window
		static_cast<DisplayManager*>(glfwGetWindowUserPointer(window))->updateViewport();
	}

	void DisplayManager::updateViewport() {
		// Get the window size and call glViewport()
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}

	GLFWmonitor* DisplayManager::bestMonitor() {
		// If for some reason we don't find a monitor, we will use primary
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();

		// Get a list of monitors
		int nmonitors;
		GLFWmonitor** monitors = glfwGetMonitors(&nmonitors);

		// We don't want to reallocate these for every monitor
		const GLFWvidmode* mode;
		int monitorX;
		int monitorY;
		int monitorWidth;
		int monitorHeight;

		// Find the center of the window, these should be set from the fullscreen method
		int centerX = windowedX + windowedWidth / 2;
		int centerY = windowedY + windowedHeight / 2;

		// Iterate over the monitors
		for (int i = 0; i < nmonitors; i++) {
			// Get monitor position data
			mode = glfwGetVideoMode(monitors[i]);
			glfwGetMonitorPos(monitors[i], &monitorX, &monitorY);
			monitorWidth = mode->width;
			monitorHeight = mode->height;

			// Does this monitor contain window center?
			if (centerX > monitorX && centerY > monitorY &&
				centerX < monitorX + monitorWidth && centerY < monitorY + monitorHeight) {
				monitor = monitors[i];
				break;
			}
		}

		return monitor;
	}
}
