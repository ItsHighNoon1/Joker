#pragma once

#include <map>
#include <vector>

#include <OpenAL/alc.h>

#include "io/Loader.h"
#include "game/Sound.h"

namespace Joker {
	class AudioManager {
		// Handle for all things OpenAL
	public:
		AudioManager(Loader loader);
		~AudioManager();
		void playSound(Sound& s);
		void tick(glm::vec3& position, float rotX, float rotY);
	private:
		ALCdevice* device;
		ALCcontext* context;
		Loader soundLoader;
		std::map<ALuint, Sound*> playing;
		std::vector<ALuint> availableSources; // Sacrifice memory in the interest of less computations
		std::vector<ALuint> toReclaim;
	};
}
