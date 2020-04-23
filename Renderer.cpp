#include "Renderer.h"

#include <glad/glad.h>

#include "Model.h"

namespace Joker {
    void Renderer::prepare() {
        // Clear the screen
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void Renderer::render(Mesh m) {
        glBindVertexArray(m.vaoID); // Bind
        glEnableVertexAttribArray(0); // Enable position buffer
        glDrawElements(GL_TRIANGLES, m.vertexCount, GL_UNSIGNED_INT, 0); // Draw
        glDisableVertexAttribArray(0); // Disable position buffer
        glBindVertexArray(0); // Unbind
    }
}
