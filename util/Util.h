#pragma once

#include <glm/glm.hpp>

#include "render/Renderable.h"
#include "util/Allocator.h"

namespace Joker {
	// Helper functions that don't belong in any particular class
	float barryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos);
	bool isBigEndian();
	int convertToInt(char* buffer, int len);

	// Custom file IO
	//Mesh loadFromOBJ(const char* path, Allocator& allocator);
	uint32_t loadFromWAV(const char* path, Allocator& allocator);
	std::map<uint8_t, FontChar> loadFontData(const char* path);
}