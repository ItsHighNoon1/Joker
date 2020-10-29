#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "io/InputHandler.h"

namespace Joker {
	class DisplayManager {
		// Responsible for the window 
	public:
		DisplayManager(int width, int height, const char* name);
		~DisplayManager();
		bool update();
		bool getKey(int key);
		bool getMouseButton(int button);
		void getMousePos(float* x, float* y);
		void fullscreen(bool fullscreen);
		float getLastDeltaTime();
		int getWidth();
		int getHeight();
		bool getFullscreen();
		InputHandler& getInputHandler();
	private:
		static void resizeCallback(GLFWwindow* window, int width, int height);
		void updateViewport();
		GLFWmonitor* bestMonitor();

		GLFWwindow* window;
		int width;
		int height;
		int windowedX;
		int windowedY;
		int windowedWidth;
		int windowedHeight;
		long long int lastTime;
		float frameTime = 1.0f;
		InputHandler inputHandler;
	};
}