#pragma once

#include <glm/glm.hpp>
#include <OpenAL/al.h>

namespace Joker {
	struct Sound {
		// Keep all the data we need to play a sound
		ALuint buffer;
		glm::vec3* position;
	};
}
