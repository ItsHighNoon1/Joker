#include "DisplayManager.h"

#include <time.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Joker {
    void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
        // When the window changes, tell OpenGL to resize
        glViewport(0, 0, width, height);
    }

    DisplayManager::DisplayManager() {
        // Initialize GLFW
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        // Create the window
        window = glfwCreateWindow(800, 500, "Joker", NULL, NULL);
        if (window == NULL) {
            glfwTerminate();
        }
        glfwMakeContextCurrent(window);

        // Load OpenGL and extensions
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            // If OpenGL failed to load, go ahead and destroy GLFW, we can't stop the program from here so we will hope OpenGL crashes
            glfwTerminate();
            return;
        }

        // Tell OpenGL the viewport size
        glViewport(0, 0, 800, 500);

        // Register a callback for window resize events so we keep our GL canvas the right size
        glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

        // Construct an InputHandler which the application can grab later
        input = InputHandler(window, 800, 500);
    }

    DisplayManager::~DisplayManager() {
        glfwTerminate();
    }

    void DisplayManager::updateDisplay() {
        glfwSwapBuffers(window); // Display the OpenGL canvas
        glfwPollEvents(); // Query for inputs

        // Check how long that frame was
        clock_t now = clock();
        long deltaTime = now - lastTime;
        dt = (float)deltaTime / 1000.0f;
        lastTime = now;

        // Calculate the per-frame distance the mouse has moved
        float xNow;
        float yNow;
        input.getRawMousePosition(xNow, yNow);
        input.dx = xNow - lastMouseX;
        input.dy = yNow - lastMouseY;
        lastMouseX = xNow;
        lastMouseY = yNow;
    }

    bool DisplayManager::shouldClose() {
        return glfwWindowShouldClose(window);
    }
}