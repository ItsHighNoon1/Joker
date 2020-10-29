#include "Util.h"

#include <fstream>
#include <sstream>

#include <OpenAL/al.h>

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
            //JK_CORE_WARN("File is not a WAV file ({0})", path);
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
            //JK_CORE_WARN("Failed to open file ({0})", path);
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