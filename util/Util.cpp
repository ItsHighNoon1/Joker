#include "Util.h"

#include <fstream>
#include <sstream>

#include <OpenAL/al.h>

#include "debug/Log.h"

namespace Joker {
    float barryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos) {
        float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
        float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
        float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
        float l3 = 1.0f - l1 - l2;
        return l1 * p1.y + l2 * p2.y + l3 * p3.y;
    }

    bool isBigEndian() {
        int a = 1;
        return !((char*)&a)[0];
    }

    int convertToInt(char* buffer, int len) {
        // Convert some chars into a float
        int a = 0;
        if (!isBigEndian()) {
            for (int i = 0; i < len; i++) {
                ((char*)&a)[i] = buffer[i];
            }
        } else {
            for (int i = 0; i < len; i++) {
                ((char*)&a)[3 - i] = buffer[i];
            }
        }
        return a;
    }

    Mesh loadFromOBJ(const char* path, Allocator& allocator) {
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

        std::map<std::string, uint32_t> uniqueVerts;
        uint32_t vertexPointer = 0;

        std::vector<float> positions;
        std::vector<float> texCoords;
        std::vector<float> normals;
        std::vector<uint32_t> indices;

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
            } else if (bit.compare("vn") == 0) {
                // The line describes a normal, which is 3 parts
                for (uint32_t i = 0; i < 3; i++) {
                    std::getline(lineStream, bit, ' ');
                    rawNormals.push_back(std::stof(bit));
                }
            } else if (bit.compare("f") == 0) {
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

        // Create mesh object
        uint32_t vaoID = allocator.loadToVAO(positions.data(), texCoords.data(), normals.data(), indices.data(), (uint32_t)indices.size(), vertexPointer);
        Mesh m;
        m.vaoID = vaoID;
        m.vertexCount = (uint32_t)indices.size();

        return m;
    }

    uint32_t loadFromWAV(const char* path, Allocator& allocator) {
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
        ALuint audioBuffer = allocator.loadAudioBuffer(data, format, size, sampleRate);
        delete[] data;

        return audioBuffer;
    }

    std::map<uint8_t, FontChar> loadFontData(const char* path) {
        // Note: there an ungodly amount of spaces in .fnt files, C++ will flip so clean them up manually lol
        std::ifstream fileStream(path, std::ios::in);
        std::map<uint8_t, FontChar> font;

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
                // I lied, the 'common' line has three pieces of info we care about
                if (bit.compare("common") == 0) {
                    std::getline(lineStream, bit, '='); // Read to the lineHeight=
                    std::getline(lineStream, bit, ' ');
                    FontChar newline;
                    newline.xAdvance = (float)std::stoi(bit) * 8.0f / fontHeight; // x8 is a trick because it seems to work well
                    font['\n'] = newline;
                    std::getline(lineStream, bit, 'W'); // Read to the scaleW (skip base)
                    std::getline(lineStream, bit, '='); // We need to move the pointer 1 more
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

            // If this is the newline character, ignore this section, because we already handled that
            if (charId == '\n') {
                continue;
            }

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
            FontChar character;
            character.position = position;
            character.size = size;
            character.offset = offset;
            character.xAdvance = kerning * 2.0f;
            font[charId] = character;
        }

        return font;
    }
}