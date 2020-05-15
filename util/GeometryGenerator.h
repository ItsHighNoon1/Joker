#pragma once

#include <glm/glm.hpp>

#include "render/Renderable.h"
#include "util/Allocator.h"

namespace Joker {
	class Terrain {
		// Temporary class that has height data for collision detection in demos
	public:
		float getHeight(glm::vec3 pos, glm::vec3 terrainPos);
		void cleanUp();
		int vertexCount;
		float size;
		Mesh mesh;
		float* heights;
	};

	class GeometryGenerator {
		// Helper class for creating all kinds of geometry
	public:
		GeometryGenerator(Allocator& allocator);
		Terrain generateTerrain(float size, float magnitude, uint32_t vertices, uint32_t texRepeats);
	private:
		Allocator& loader;
	};
}