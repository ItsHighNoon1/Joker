#pragma once

#include <AL/al.h>
#include <glm/glm.hpp>

namespace Joker {
	struct Sound {
		// Keep all the data we need to play a sound
		ALuint buffer;
		glm::vec3* position;
	};
}