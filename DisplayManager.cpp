#include "DisplayManager.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Joker {
    void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
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
    }

    DisplayManager::~DisplayManager() {
        glfwTerminate();
    }

    void DisplayManager::updateDisplay() {
        // Get the next framebuffer from the swapchain and display it
        glfwSwapBuffers(window);
        // Get events like window closing, resizing, etc.
        glfwPollEvents();
    }

    bool DisplayManager::shouldClose() {
        return glfwWindowShouldClose(window);
    }
}