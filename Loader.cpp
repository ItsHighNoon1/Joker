#include "Loader.h"

#include <vector>

#include <iostream>

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace Joker {
    Loader::Loader() {
        // OpenGL expects textures to be in a certain format, and this will do that for us
        stbi_set_flip_vertically_on_load(true);
    }

    Mesh Loader::loadToVAO(GLfloat* positions, GLfloat* texCoords, GLuint* indices, GLsizei count) {
        // We have everything we need to make the struct immediately, so do that first
        Mesh m;
        m.vaoID = createVAO();
        m.vertexCount = count;

        // Put our vertex data into memory
        bindIndicesBuffer(indices, sizeof(GLuint) * count); // TODO size is kinda hacky here
        storeDataInAttributeList(0, positions, sizeof(GLfloat) * count * 3, 3);
        storeDataInAttributeList(1, texCoords, sizeof(GLfloat) * count * 2, 2);

        // Unbind so nobody modifies
        glBindVertexArray(0);
        return m;
    }

    GLuint Loader::loadTexture(const char* path) {
        // Read in a texture from the file system
        int width;
        int height;
        int channels;
        unsigned char* data = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);

        // Allocate a space in OpenGL
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Load the data into OpenGL and manage memory
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
        textures.push_back(texture);

        // Set a couple parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Wrap the texture if there is repeat
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Pixel when you are close
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Mipmap when you are far
        glGenerateMipmap(GL_TEXTURE_2D); // We need mipmaps

        return texture;
    }

    void Loader::cleanUp() {
        // Delete objects stored in memory
        for (uint32_t i = 0; i < vaos.size(); i++) {
            glDeleteVertexArrays(1, &vaos[i]);
        }
        for (uint32_t i = 0; i < vbos.size(); i++) {
            glDeleteBuffers(1, &vbos[i]);
        }
        for (uint32_t i = 0; i < textures.size(); i++) {
            glDeleteBuffers(1, &textures[i]);
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