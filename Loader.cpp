#include "Loader.h"

#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <glad/glad.h>
#include <OpenAL/al.h>

#include "Object.h"
#include "Util.h"
#include "Log.h"

namespace Joker {
    Loader::Loader() {
        // OpenGL expects textures to be in a certain format, and this will do that for us
        stbi_set_flip_vertically_on_load(true);
    }

    Mesh Loader::loadToVAO(GLfloat* positions, GLfloat* texCoords, GLfloat* normals, GLuint* indices, GLsizei count, GLsizei uniqueVertices) {
        // Generate our VAO
        GLuint vaoID;
        glGenVertexArrays(1, &vaoID);
        vaos.push_back(vaoID);
        glBindVertexArray(vaoID);

        // We have everything we need to make the struct immediately, so do that first
        Mesh m;
        m.vaoID = vaoID;
        m.vertexCount = count;

        // Put our vertex data into memory
        bindIndicesBuffer(indices, sizeof(GLuint) * count);
        storeDataInAttributeList(0, positions, sizeof(GLfloat) * uniqueVertices * 3, 3);
        storeDataInAttributeList(1, texCoords, sizeof(GLfloat) * uniqueVertices * 2, 2);
        storeDataInAttributeList(2, normals, sizeof(GLfloat) * uniqueVertices * 3, 3);

        // Unbind so nobody modifies
        glBindVertexArray(0);
        return m;
    }

    Mesh Loader::loadQuad() {
        // Do everything you normally would
        GLuint vaoID;
        glGenVertexArrays(1, &vaoID);
        vaos.push_back(vaoID);
        glBindVertexArray(vaoID);
        Mesh m;
        m.vaoID = vaoID;
        m.vertexCount = 6; // 1 quad is 2 tris is 6 verts

        // Quad data, if you're rendering a quad with lighting you're doing something wrong so we don't consider normals
        GLuint indices[] = {
            0, 2, 1, 
            0, 3, 2
        };
        GLfloat positions[] = {
            -1.0f, -1.0f, 
            -1.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, -1.0f
        };
        GLfloat texCoords[] = {
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
        };
        bindIndicesBuffer(indices, sizeof(GLuint) * 6);
        storeDataInAttributeList(0, positions, sizeof(GLfloat) * 4 * 2, 2);
        storeDataInAttributeList(1, texCoords, sizeof(GLfloat) * 4 * 2, 2);

        glBindVertexArray(0);
        return m;
    }

    Mesh Loader::loadFromOBJ(const char* path) {
        // As always, the OBJ loader is the most complicated method
        std::ifstream fileStream(path, std::ios::in);

        // If the file couldn't be opened, return a null mesh
        if (!fileStream.is_open()) {
            JK_CORE_WARN("Failed to open file ({0})", path);
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

                        // Find which position, texture coord, and normal belong to this vertex
                        // Casts are because std::stoi returns a 32-bit int, but .at wants 64 bits
                        std::getline(bitStream, index, '/');
                        size_t posIndex = ((size_t)std::stoi(index) - 1) * 3;
                        std::getline(bitStream, index, '/');
                        size_t texIndex = ((size_t)std::stoi(index) - 1) * 2;
                        std::getline(bitStream, index, '/');
                        size_t normIndex = ((size_t)std::stoi(index) - 1) * 3;

                        // Push those values to the VBO
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
        return loadToVAO(positions.data(), texCoords.data(), normals.data(), indices.data(), (GLsizei)indices.size(), (GLsizei)vertexPointer);
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glGenerateMipmap(GL_TEXTURE_2D); // We need mipmaps

        return texture;
    }

    Framebuffer Loader::loadFramebuffer(GLsizei width, GLsizei height) {
        // Generate a framebuffer in OpenGL
        GLuint framebufferID = 0;
        glGenFramebuffers(1, &framebufferID);
        framebuffers.push_back(framebufferID);
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);

        // Create a color buffer texture
        GLuint colorbuffer;
        glGenTextures(1, &colorbuffer);
        textures.push_back(colorbuffer);
        glBindTexture(GL_TEXTURE_2D, colorbuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0); // Empty texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorbuffer, 0);

        // Depth/stencil buffer
        GLuint depthbuffer;
        glGenTextures(1, &depthbuffer);
        textures.push_back(depthbuffer);
        glBindTexture(GL_TEXTURE_2D, depthbuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthbuffer, 0);

        // Check that nothing went wrong
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            JK_CORE_ERROR("Failed to generate framebuffer");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // Create a framebuffer object
        Framebuffer buffer;
        buffer.width = width;
        buffer.height = height;
        buffer.buffer = framebufferID;
        buffer.colorbuffer = colorbuffer;
        buffer.depthbuffer = depthbuffer;

        return buffer;
    }

    ALuint Loader::loadAudioBuffer(char* data, ALenum format, ALsizei size, ALsizei freq) {
        ALuint buffer;
        alGenBuffers(1, &buffer);
        audioBuffers.push_back(buffer);
        alBufferData(buffer, format, data, size, freq);
        return buffer;
    }

    ALuint Loader::loadFromWAV(const char* path) {
        int channel;
        int sampleRate;
        int bps;
        int size;

        // Read the raw file because we're crazy
        // https://stackoverflow.com/questions/36949957/loading-a-wav-file-for-openal
        char buffer[4];
        std::ifstream in(path, std::ios::binary);
        in.read(buffer, 4);
        if (strncmp(buffer, "RIFF", 4) != 0) {
            JK_CORE_WARN("File is not a WAV file ({0})", path);
        }

        // Skip the header stuff
        in.read(buffer, 4);
        in.read(buffer, 4);
        in.read(buffer, 4);
        in.read(buffer, 4);
        in.read(buffer, 2);

        // Read the metadata, we ignore most of it but gather some key pieces
        in.read(buffer, 2);
        channel = convertToInt(buffer, 2);
        in.read(buffer, 4);
        sampleRate = convertToInt(buffer, 4);
        in.read(buffer, 4);
        in.read(buffer, 2);
        in.read(buffer, 2);
        bps = convertToInt(buffer, 2);
        in.read(buffer, 4);
        in.read(buffer, 4);
        size = convertToInt(buffer, 4);

        // Read the sound data
        char* data = new char[size];
        in.read(data, size);

        // OpenAL needs to know what format this sound is in
        ALenum format;
        if (channel == 1) {
            if (bps == 8) {
                format = AL_FORMAT_MONO8;
            } else {
                format = AL_FORMAT_MONO16;
            }
        } else {
            if (bps == 8) {
                format = AL_FORMAT_STEREO8;
            } else {
                format = AL_FORMAT_STEREO16;
            }
        }

        // We can't just return the loaded audio buffer because we still need to clean up data
        ALuint audioBuffer = loadAudioBuffer(data, format, size, sampleRate);
        delete[] data;

        return audioBuffer;
    }

    ALuint Loader::createSource() {
        ALuint source;
        alGenSources(1, &source);
        audioSources.push_back(source);
        return source;
    }

    std::map<uint8_t, TextChar> Loader::loadFont(const char* path) {
        // Note: there an ungodly amount of spaces in .fnt files, C++ will flip so clean them up manually lol
        std::ifstream fileStream(path, std::ios::in);
        std::map<uint8_t, TextChar> font;

        // If the file couldn't be opened, return nothing
        if (!fileStream.is_open()) {
            JK_CORE_WARN("Failed to open file ({0})", path);
            return font;
        }

        // It's a surprise tool that will help us later
        float fontWidth = 512.0f;
        float fontHeight = 512.0f;

        // Read line by line
        for (std::string line; std::getline(fileStream, line); ) {
            std::stringstream lineStream(line);
            std::string bit;
            std::getline(lineStream, bit, ' '); // Trick used because C++ has no string split

            // Every line we care about starts with 'char', so if this one doesn't, skip it
            if (bit.compare("char") != 0) {
                // I lied, the 'common' line has two pieces of info we care about
                if (bit.compare("common") == 0) {
                    std::getline(lineStream, bit, 'W'); // Read to the scaleW
                    std::getline(lineStream, bit, '='); // This is horrible
                    std::getline(lineStream, bit, ' ');
                    fontWidth = (float)std::stoi(bit);
                    std::getline(lineStream, bit, '='); // scaleH=
                    std::getline(lineStream, bit, ' ');
                    fontHeight = (float)std::stoi(bit);
                }
                continue;
            }
            std::getline(lineStream, bit, '='); // Read to the id=
            std::getline(lineStream, bit, ' '); // Read whatever the number is
            uint8_t charId = std::stoi(bit); // This is the character ID, which we will need later

            // Read the position using a similar trick
            glm::vec2 position;
            std::getline(lineStream, bit, '=');
            std::getline(lineStream, bit, ' ');
            position.x = std::stoi(bit) / fontWidth;
            std::getline(lineStream, bit, '=');
            std::getline(lineStream, bit, ' ');
            position.y = 1.0f - std::stoi(bit) / fontHeight;

            // Size
            glm::vec2 size;
            std::getline(lineStream, bit, '=');
            std::getline(lineStream, bit, ' ');
            size.x = std::stoi(bit) / fontWidth;
            std::getline(lineStream, bit, '=');
            std::getline(lineStream, bit, ' ');
            size.y = std::stoi(bit) / fontHeight;

            // Offset
            glm::vec2 offset;
            std::getline(lineStream, bit, '=');
            std::getline(lineStream, bit, ' ');
            offset.x = std::stoi(bit) / fontWidth;
            std::getline(lineStream, bit, '=');
            std::getline(lineStream, bit, ' ');
            offset.y = std::stoi(bit) / fontHeight;

            // Kerning
            std::getline(lineStream, bit, '=');
            std::getline(lineStream, bit, ' ');
            float kerning = std::stoi(bit) / fontWidth;


            // Move the texture coord to the bottom of the character instead of the top
            position.y -= size.y;

            // The rest is probably useful, but I'm lazy so let's call this good
            TextChar character;
            character.position = position;
            character.size = size;
            character.offset = offset;
            character.xAdvance = kerning;
            font[charId] = character;
        }

        return font;
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
        for (uint32_t i = 0; i < framebuffers.size(); i++) {
            glDeleteFramebuffers(1, &framebuffers[i]);
        }
        for (uint32_t i = 0; i < audioBuffers.size(); i++) {
            alDeleteBuffers(1, &audioBuffers[i]);
        }
        for (uint32_t i = 0; i < audioSources.size(); i++) {
            alDeleteSources(1, &audioSources[i]);
        }
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
