#pragma once

#include <map>
#include <vector>

#include <glm/glm.hpp>
#include <OpenAL/al.h>

#include "game/Sound.h"
#include "util/Allocator.h"

namespace Joker {
	class AudioManager {
		// Handle for all things OpenAL
	public:
		AudioManager(Allocator& allocator);
		~AudioManager();
		void playSound(Sound& s);
		void tick(glm::vec3& position, float rotX, float rotY);
	private:
		ALCdevice* device;
		ALCcontext* context;
		Allocator& soundLoader;
		std::map<ALuint, Sound*> playing;
		std::vector<ALuint> availableSources; // Sacrifice memory in the interest of less computations
		std::vector<ALuint> toReclaim;
	};
}
