#include "Renderer.h"

#include <glad/glad.h>

#include "Model.h"
#include "Shader.h"

namespace Joker {
    void Renderer::prepare() {
        // Clear the screen
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // This is state based, but it doesn't cost any time so set it every frame anyways
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }
}
