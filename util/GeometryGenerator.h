#pragma once

#include "util/Allocator.h"

namespace Joker {
	class GeometryGenerator {
		// Helper class for creating all kinds of geometry
	public:
		GeometryGenerator(Allocator& allocator);
		Mesh generateTerrain(float size, uint32_t vertices, uint32_t texRepeats);
	private:
		Allocator& loader;
	};
}