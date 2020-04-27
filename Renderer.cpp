#include "Renderer.h"

#include <glad/glad.h>

#include "Model.h"
#include "Shader.h"

namespace Joker {
    void Renderer::prepare() {
        // Set a bunch of flags for the renderer in case some other application has changed them (is that possible?)
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen
    }
}
