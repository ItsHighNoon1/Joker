#include "Renderer.h"

#include <glad/glad.h>

namespace Joker {
    void Renderer::prepare() {
        // Set a bunch of flags for the renderer in case some other application has changed them (is that possible?)
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::enableDepthTest() {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }

    void Renderer::disableDepthTest() {
        glDisable(GL_DEPTH_TEST);
    }
}
