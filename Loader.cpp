#include "Loader.h"

#include <vector>

#include <iostream>

#include <glad/glad.h>

namespace Joker {
    Mesh Loader::loadToVAO(GLfloat* positions, GLuint* indices, GLsizei count) {
        // We have everything we need to make the struct immediately, so do that first
        Mesh m;
        m.vaoID = createVAO();
        m.vertexCount = count;

        // Put our vertex data into memory
        bindIndicesBuffer(indices, sizeof(GLuint) * count); // TODO size is kinda hacky here
        storeDataInAttributeList(0, positions, sizeof(GLfloat) * count * 3, 3); // TODO size is kinda hacky here

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

    void Loader::storeDataInAttributeList(GLuint attributeNumber, GLfloat* data, GLsizeiptr totalSize, GLsizei vertexLength) {
        GLuint vboID;
        glGenBuffers(1, &vboID); // Allocate
        vbos.push_back(vboID); // Track
        glBindBuffer(GL_ARRAY_BUFFER, vboID); // Bind
        glBufferData(GL_ARRAY_BUFFER, totalSize, data, GL_STATIC_DRAW); // Fill
        glVertexAttribPointer(attributeNumber, vertexLength, GL_FLOAT, GL_FALSE, 0, nullptr); // Data used at draw time
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind
    }

    void Loader::bindIndicesBuffer(GLuint* indices, GLsizeiptr size) {
        GLuint vboID;
        glGenBuffers(1, &vboID); // Allocate
        vbos.push_back(vboID); // Track
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID); // Bind
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW); // Fill
    }
}