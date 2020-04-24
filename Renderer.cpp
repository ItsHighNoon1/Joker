#include "Renderer.h"

#include <glad/glad.h>

#include "Model.h"
#include "Shader.h"

namespace Joker {
    void Renderer::prepare() {
        // Clear the screen
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // This is state based, but it doesn't cost any time so set it every frame anyways
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::render(Shader& shader, Mesh& mesh) {
        //shader.start();
        glBindVertexArray(mesh.vaoID); // Bind
        glEnableVertexAttribArray(0); // Enable position buffer
        glDrawElements(GL_TRIANGLES, mesh.vertexCount, GL_UNSIGNED_INT, 0); // Draw
        glDisableVertexAttribArray(0); // Disable position buffer
        glBindVertexArray(0); // Unbind
        //shader.stop();
    }
}
