#include "Loader.h"

#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace Joker {
    Loader::Loader() {
        // OpenGL expects textures to be in a certain format, and this will do that for us
        stbi_set_flip_vertically_on_load(true);
    }

    Mesh Loader::loadToVAO(GLfloat* positions, GLfloat* texCoords, GLfloat* normals, GLuint* indices, GLsizei count) {
        // We have everything we need to make the struct immediately, so do that first
        Mesh m;
        m.vaoID = createVAO();
        m.vertexCount = count;

        // Put our vertex data into memory
        bindIndicesBuffer(indices, sizeof(GLuint) * count); // TODO size is kinda hacky here
        storeDataInAttributeList(0, positions, sizeof(GLfloat) * count * 3, 3);
        storeDataInAttributeList(1, texCoords, sizeof(GLfloat) * count * 2, 2);
        storeDataInAttributeList(2, normals, sizeof(GLfloat) * count * 3, 3);

        // Unbind so nobody modifies
        glBindVertexArray(0);
        return m;
    }

    Mesh Loader::loadFromOBJ(const char* path) {
        // As always, the OBJ loader is the most complicated method
        std::ifstream fileStream(path, std::ios::in);

        // If the file couldn't be opened, return a null mesh
        if (!fileStream.is_open()) {
            printf("Couldn't find file: %s\n", path);
            Mesh m;
            m.vaoID = 0;
            m.vertexCount = 0;
            return m;
        }
        
        // We need a ton of vectors to throw data into during our parsing
        std::vector<float> rawPositions;
        std::vector<float> rawTexCoords;
        std::vector<float> rawNormals;
        std::vector<uint32_t> rawIndices;

        std::map<std::string, GLuint> uniqueVerts;
        GLuint vertexPointer = 0;

        std::vector<GLfloat> positions;
        std::vector<GLfloat> texCoords;
        std::vector<GLfloat> normals;
        std::vector<GLuint> indices;

        // Parse the file
        for (std::string line; std::getline(fileStream, line); ) {
            std::stringstream lineStream(line);
            std::string bit;
            std::getline(lineStream, bit, ' '); // Trick used because C++ has no string split
            
            // Big line parsing
            if (bit.compare("v") == 0) {
                // The line describes a vertex, which is 3 parts
                for (uint32_t i = 0; i < 3; i++) {
                    std::getline(lineStream, bit, ' ');
                    rawPositions.push_back(std::stof(bit));
                }
            } else if (bit.compare("vt") == 0) {
                // The line describes a texture coordinate, which is 2 parts
                for (uint32_t i = 0; i < 2; i++) {
                    std::getline(lineStream, bit, ' ');
                    rawTexCoords.push_back(std::stof(bit));
                }
            } else if(bit.compare("vn") == 0) {
                // The line describes a normal, which is 3 parts
                for (uint32_t i = 0; i < 3; i++) {
                    std::getline(lineStream, bit, ' ');
                    rawNormals.push_back(std::stof(bit));
                }
            } else if(bit.compare("f") == 0) {
                // The line describes a face, so it's time to start building the model
                for (uint32_t i = 0; i < 3; i++) {
                    std::getline(lineStream, bit, ' ');
                    if (uniqueVerts.find(bit) != uniqueVerts.end()) {
                        // We have already indexed this vertex, so throw whatever index we gave it onto the list
                        indices.push_back(uniqueVerts.at(bit));
                    } else {
                        // This is a new vertex
                        indices.push_back(vertexPointer);
                        uniqueVerts[bit] = vertexPointer;
                        vertexPointer++;

                        // We need to add the appropriate raw data into the structured lists
                        std::stringstream bitStream(bit);
                        std::string index;

                        // Add the vertex to the VBO lists
                        std::getline(bitStream, index, '/');
                        uint32_t posIndex = (std::stoi(index) - 1) * 3;
                        std::getline(bitStream, index, '/');
                        uint32_t texIndex = (std::stoi(index) - 1) * 2;
                        std::getline(bitStream, index, '/');
                        uint32_t normIndex = (std::stoi(index) - 1) * 3;

                        positions.push_back(rawPositions.at(posIndex));
                        positions.push_back(rawPositions.at(posIndex + 1));
                        positions.push_back(rawPositions.at(posIndex + 2));
                        texCoords.push_back(rawTexCoords.at(texIndex));
                        texCoords.push_back(rawTexCoords.at(texIndex + 1));
                        normals.push_back(rawNormals.at(normIndex));
                        normals.push_back(rawNormals.at(normIndex + 1));
                        normals.push_back(rawNormals.at(normIndex + 2));
                    }
                }
            }
        }

        fileStream.close();
        return loadToVAO(positions.data(), texCoords.data(), normals.data(), indices.data(), (GLsizei)indices.size());
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