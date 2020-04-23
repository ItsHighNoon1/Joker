#include "Loader.h"

#include <vector>

#include <glad/glad.h>

namespace Joker {
    Mesh Loader::loadToVAO(float positions[], int indices[], int count) {
        // We have everything we need to make the struct immediately, so do that first
        Mesh m;
        m.vaoID = createVAO();
        m.vertexCount = count;

        // Put our vertex data into memory
        bindIndicesBuffer(indices);
        storeDataInAttributeList(0, positions, 3);

        // Unbind so nobody modifies
        glBindVertexArray(0);
        return m;
    }

    void Loader::cleanUp() {
        // Delete objects stored in memory
        for (uint32_t i = 0; i < vaos.size(); i++) {
            glDeleteVertexArrays(1, &vaos[i]);
        }
        for (uint32_t i = 0; i < vbos.size(); i++) {
            glDeleteBuffers(1, &vbos[i]);
        }
    }

    GLuint Loader::createVAO() {
        GLuint vaoID;
        glGenVertexArrays(1, &vaoID); // Allocate
        vaos.push_back(vaoID); // Track (for cleanup)
        glBindVertexArray(vaoID); // Bind (redundant in some impls, but safety)
        return vaoID;
    }

    void Loader::storeDataInAttributeList(GLuint attributeNumber, float data[], GLsizei size) {
        GLuint vboID;
        glGenBuffers(1, &vboID); // Allocate
        vbos.push_back(vboID); // Track
        glBindBuffer(GL_ARRAY_BUFFER, vboID); // Bind
        glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW); // Fill
        glVertexAttribPointer(attributeNumber, size, GL_FLOAT, GL_FALSE, 0, nullptr); // Data used at draw time
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind
    }

    void Loader::bindIndicesBuffer(int indices[]) {
        GLuint vboID;
        glGenBuffers(1, &vboID); // Allocate
        vbos.push_back(vboID); // Track
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID); // Bind
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Fill
    }
}