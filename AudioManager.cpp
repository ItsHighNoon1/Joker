#include "AudioManager.h"

#include <iostream>

#include <AL/al.h>
#include <AL/alc.h>

#include "Loader.h"
#include "Sound.h"

namespace Joker {
    AudioManager::AudioManager(Loader loader) {
        // Open a sound device
        device = alcOpenDevice(nullptr);
        if (device == nullptr) {
            std::cout << "Could not open sound device!" << std::endl;
            return;
        }

        // Create the audio rendering context
        context = alcCreateContext(device, nullptr);
        if (context == nullptr) {
            std::cout << "cannot open context" << std::endl;
            return;
        }
        alcMakeContextCurrent(context);

        // We need to keep track of the loader just in case we need to generate sources in realtime
        soundLoader = loader;
    }

    AudioManager::~AudioManager() {
        alcDestroyContext(context);
        alcCloseDevice(device);
    }

    void AudioManager::playSound(Sound& s) {
        // First, we need to 
        ALuint source;
        if (availableSources.size() > 0) {
            // There is an available source, so use it and then delete it from the list
            source = availableSources[0];
            availableSources.erase(availableSources.begin());
        } else {
            // If there are no available sources, allocate a new one
            source = soundLoader.createSource();
        }
        playing[source] = &s;

        // Actually play the sound
        alSourcei(source, AL_BUFFER, s.buffer);
        alSourcePlay(source);
    }

    void AudioManager::tick(glm::vec3& position, float rotX, float rotY) {
        // Update the listener's position and orientation
        alListener3f(AL_POSITION, position.x, position.y, position.z);
        float sinX = sinf(rotX);
        float sinY = sinf(rotY);
        float cosX = cosf(rotX);
        float cosY = cosf(rotY);
        ALfloat orientationBuffer[] = { // Don't even ask
            sinY* cosX, sinX, -cosX * cosY,
            -sinX * sinY, cosX, sinX* cosY
        };
        alListenerfv(AL_ORIENTATION, orientationBuffer);

        // Iterate through all the sounds in case we need to do anything to them
        for (auto const& [source, sound] : playing) {
            // Figure out if the sound is still playing
            ALint state;
            alGetSourcei(source, AL_SOURCE_STATE, &state);
            if (state == AL_PLAYING) {
                // If the sound is still playing, update its source's attributes
                alSource3f(source, AL_POSITION, sound->position->x, sound->position->y, sound->position->z);
            } else {
                // If the sound has ended, remove the entry and add the source back to the pool
                toReclaim.push_back(source);
                availableSources.push_back(source);
            }
        }

        // Remove everything that we've queued to remove
        for (uint32_t i = 0; i < toReclaim.size(); i++) {
            playing.erase(toReclaim[i]);
        }
        toReclaim.clear();
    }
}