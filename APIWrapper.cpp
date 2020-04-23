#include "APIWrapper.h"

#include <spdlog/spdlog.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Joker {
    APIWrapper::APIWrapper() {
        // Initialize GLFW
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        // Create the window
        window = glfwCreateWindow(800, 500, "Joker", NULL, NULL);
        if (window == NULL) {
            SPDLOG_ERROR("GLFW failed to create the window");
            glfwTerminate();
        }
        glfwMakeContextCurrent(window);

        // Load OpenGL and extensions
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            SPDLOG_ERROR("Failed to load OpenGL");
        }

        // Tell OpenGL the viewport size
        glViewport(0, 0, 800, 500);

        // Register a callback for window resize events
        glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

        // Generate the VAO
        genVAO();
	}

    APIWrapper::~APIWrapper() {
        glfwTerminate();
    }

    void APIWrapper::poll() {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    bool APIWrapper::shouldClose() {
        return glfwWindowShouldClose(window);
    }

    void APIWrapper::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
        SPDLOG_INFO("Window resized {} {}", width, height);
        glViewport(0, 0, width, height);
    }

    Mesh APIWrapper::loadToVAO() {
        Mesh m;
        // Allocate
        glGenVertexArrays(1, &m.vaoID);
        glBindVertexArray(m.vaoID);

        // Generate the position buffer
        static const GLfloat positions[] = {
           -1.0f, -1.0f, 0.0f,
           1.0f, -1.0f, 0.0f,
           0.0f,  1.0f, 0.0f,
        };
        uint32_t vboID;
        glGenBuffers(1, &vboID);
        glBindBuffer(GL_ARRAY_BUFFER, vboID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

        return m;
    }
}