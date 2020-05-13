#define STB_IMAGE_IMPLEMENTATION
#include "Joker.h"

namespace Joker {
    Allocator::Allocator() {
        // OpenGL expects textures to be in a certain format, and this will do that for us
        stbi_set_flip_vertically_on_load(true);
    }

    Allocator::~Allocator() {
        // Delete objects we have allocated (the renderer may also delete framebuffers, this isn't a problem)
        glDeleteVertexArrays((uint32_t)vaos.size(), vaos.data());
        glDeleteVertexArrays((uint32_t)vbos.size(), vbos.data());
        glDeleteVertexArrays((uint32_t)textures.size(), textures.data());
        glDeleteVertexArrays((uint32_t)framebuffers.size(), framebuffers.data());
        alDeleteBuffers((uint32_t)audioBuffers.size(), audioBuffers.data());
        alDeleteSources((uint32_t)audioSources.size(), audioSources.data());
    }

    uint32_t Allocator::loadToVAO(float* positions, float* texCoords, float* normals, uint32_t* indices, uint32_t count, uint32_t uniqueVertices) {
        // Generate our VAO
        uint32_t vaoID;
        glGenVertexArrays(1, &vaoID);
        vaos.push_back(vaoID);
        glBindVertexArray(vaoID);

        // Put our vertex data into memory
        bindIndicesBuffer(indices, sizeof(GLuint) * count);
        storeDataInAttributeList(0, positions, sizeof(GLfloat) * uniqueVertices * 3, 3);
        storeDataInAttributeList(1, texCoords, sizeof(GLfloat) * uniqueVertices * 2, 2);
        storeDataInAttributeList(2, normals, sizeof(GLfloat) * uniqueVertices * 3, 3);

        // Unbind so nobody modifies, since the application is probably loading a lot of things right now
        glBindVertexArray(0);
        return vaoID;
    }

    uint32_t Allocator::loadQuad() {
        // Do everything you normally would
        uint32_t vaoID;
        glGenVertexArrays(1, &vaoID);
        vaos.push_back(vaoID);
        glBindVertexArray(vaoID);

        // Quad data, if you're rendering a quad with lighting you're doing something wrong so we don't consider normals
        uint32_t indices[] = {
            0, 2, 1,
            0, 3, 2
        };
        float positions[] = {
            -1.0f, -1.0f,
            -1.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, -1.0f
        };
        float texCoords[] = {
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
        };
        bindIndicesBuffer(indices, sizeof(uint32_t) * 6);
        storeDataInAttributeList(0, positions, sizeof(float) * 4 * 2, 2);
        storeDataInAttributeList(1, texCoords, sizeof(float) * 4 * 2, 2);

        glBindVertexArray(0);
        return vaoID;
    }

    uint32_t Allocator::loadCube() {
        // Make a box, because the skybox is a box
        uint32_t vaoID;
        glGenVertexArrays(1, &vaoID);
        vaos.push_back(vaoID);
        glBindVertexArray(vaoID);

        // Cube data, we don't even need texture coords because skybox coords just come from the position
        uint32_t indices[] = {
            2, 1, 0,
            0, 3, 2,
            6, 5, 1,
            1, 2, 6,
            5, 6, 7,
            7, 4, 5,
            3, 0, 4,
            4, 7, 3,
            1, 5, 4,
            4, 0, 1,
            6, 2, 3,
            3, 7, 6
        };
        float positions[] = {
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f
        };
        bindIndicesBuffer(indices, sizeof(uint32_t) * 36);
        storeDataInAttributeList(0, positions, sizeof(float) * 8 * 3, 3);

        glBindVertexArray(0);
        return vaoID;
    }

    uint32_t Allocator::loadTexture(const char* path) {
        return loadTexture(path, true);
    }

    uint32_t Allocator::loadTexture(const char* path, bool linear) {
        // Read in a texture from the file system
        int width;
        int height;
        int channels;
        unsigned char* data = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);

        // Allocate a space in OpenGL
        uint32_t texture;
        glGenTextures(1, &texture);
        textures.push_back(texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Load the data into OpenGL and manage memory
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);

        // Set a couple parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Wrap the texture if there is repeat
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        if (linear) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            // Some things (fonts, sprite sheets) look worse with linear filtering, so we provide the option to turn it off
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }

        return texture;
    }

    uint32_t Allocator::loadCubeMap(const char* up, const char* down, const char* left, const char* right, const char* front, const char* back) {
        // This makes the cube map load properly in spite of my earlier decree
        stbi_set_flip_vertically_on_load(false);

        uint32_t texture;
        glGenTextures(1, &texture);
        textures.push_back(texture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture); // It's a cubemap, not a 2D

        // Read 6 textures
        int width;
        int height;
        int channels;
        unsigned char* data;

        // Forgive me, there should really be a for loop but I desire explicit control
        data = stbi_load(up, &width, &height, &channels, STBI_rgb_alpha);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
        data = stbi_load(down, &width, &height, &channels, STBI_rgb_alpha);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
        data = stbi_load(left, &width, &height, &channels, STBI_rgb_alpha);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
        data = stbi_load(right, &width, &height, &channels, STBI_rgb_alpha);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
        data = stbi_load(front, &width, &height, &channels, STBI_rgb_alpha);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
        data = stbi_load(back, &width, &height, &channels, STBI_rgb_alpha);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);

        // Texture params
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Note no mipmaps

        // Set this back in case we load more 2D textures
        stbi_set_flip_vertically_on_load(true);

        return texture;
    }

    uint32_t Allocator::loadFramebuffer(uint32_t width, uint32_t height, uint32_t* colorBuffer, uint32_t* depthBuffer) {
        // Generate a framebuffer in OpenGL
        uint32_t framebufferID = 0;
        glGenFramebuffers(1, &framebufferID);
        framebuffers.push_back(framebufferID);
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);

        // Create a color buffer texture
        uint32_t color;
        glGenTextures(1, &color);
        textures.push_back(color);
        glBindTexture(GL_TEXTURE_2D, color);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0); // Empty texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);

        // Depth/stencil buffer
        uint32_t depth;
        glGenTextures(1, &depth);
        textures.push_back(depth);
        glBindTexture(GL_TEXTURE_2D, depth);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth, 0);

        // Check that nothing went wrong
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            JK_CORE_ERROR("Failed to generate framebuffer");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Return these via pointer in case somebody needs it
        if (colorBuffer != nullptr) {
            *colorBuffer = color;
        }
        if (depthBuffer != nullptr) {
            *depthBuffer = depth;
        }
        
        return framebufferID;
    }

    uint32_t Allocator::createSource() {
        // Pretty simple because we aren't uploading any data
        uint32_t source;
        alGenSources(1, &source);
        audioSources.push_back(source);
        return source;
    }

    uint32_t Allocator::loadAudioBuffer(char* data, uint32_t format, uint32_t size, uint32_t freq) {
        // Upload data, which is also simple because there is only one buffer
        uint32_t buffer;
        alGenBuffers(1, &buffer);
        audioBuffers.push_back(buffer);
        alBufferData(buffer, format, data, size, freq);
        return buffer;
    }

    void Allocator::storeDataInAttributeList(uint32_t attributeNumber, float* data, uint32_t totalSize, uint32_t vertexLength) {
        // Upload floats to OpenGL in the form of a VBO
        glEnableVertexAttribArray(attributeNumber);
        uint32_t vboID;
        glGenBuffers(1, &vboID); // Allocate
        vbos.push_back(vboID); // Track
        glBindBuffer(GL_ARRAY_BUFFER, vboID); // Bind
        glBufferData(GL_ARRAY_BUFFER, totalSize, data, GL_STATIC_DRAW); // Fill
        glVertexAttribPointer(attributeNumber, vertexLength, GL_FLOAT, GL_FALSE, 0, nullptr); // Data used at draw time
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind
    }

    void Allocator::bindIndicesBuffer(uint32_t* indices, uint32_t size) {
        uint32_t vboID;
        glGenBuffers(1, &vboID); // Allocate
        vbos.push_back(vboID); // Track
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID); // Bind
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW); // Fill
    }
}