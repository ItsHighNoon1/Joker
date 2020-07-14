#include "DisplayManager.h"

#include "debug/Log.h"

namespace Joker {
    uint32_t DisplayManager::windowWidth;
    uint32_t DisplayManager::windowHeight;

    void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
        // When the window changes, store the new size
        DisplayManager::windowWidth = width;
        DisplayManager::windowHeight = height;
    }

    DisplayManager::DisplayManager() {
        // Initialize GLFW
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Create the window
        window = glfwCreateWindow(800, 500, "Joker 0.0.2", NULL, NULL);
        if (window == NULL) {
            JK_CORE_ERROR("Failed to initialize GLFW");
            glfwTerminate();
        }
        glfwMakeContextCurrent(window);

        // Load OpenGL and extensions
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            // If OpenGL failed to load, go ahead and destroy GLFW, we can't stop the program from here so we will hope OpenGL crashes
            JK_CORE_ERROR("Failed to load OpenGL");
            glfwTerminate();
            return;
        }

        // Tell OpenGL the viewport size
        glViewport(0, 0, 800, 500);
        windowWidth = 800;
        windowHeight = 500;

        // Register a callback for window resize events so we keep our GL canvas the right size
        glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

        // Disable Vsync for that CRISP framerate
        glfwSwapInterval(0);

        // Construct an InputHandler which the application can grab later
        input = InputHandler(window, &windowWidth, &windowHeight);
    }

    DisplayManager::~DisplayManager() {
        glfwTerminate(); // Cleans up window pointer for us
    }

    void DisplayManager::updateDisplay() {
        glfwSwapBuffers(window); // Display the OpenGL canvas
        glfwPollEvents(); // Query for inputs

        // Check how long that frame was
        float now = (float)glfwGetTime();
        dt = now - lastTime;
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
