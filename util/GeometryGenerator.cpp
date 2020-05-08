#include "GeometryGenerator.h"

#include <vector>

#define STB_PERLIN_IMPLEMENTATION
#include <stb/stb_perlin.h>

#include "Allocator.h"
#include "render/Renderable.h"

namespace Joker {
	// No body because all we need is to initialize allocator
	GeometryGenerator::GeometryGenerator(Allocator& allocator) : loader(allocator) {}

	Mesh GeometryGenerator::generateTerrain(float size, uint32_t vertices, uint32_t texRepeats) {
		// Set up variables that we will push data into
		uint32_t uniqueVerts = vertices * vertices;
		std::vector<float> positions;
		std::vector<float> texCoords;
		std::vector<float> normals;
		std::vector<uint32_t> indices;
		positions.reserve((size_t)uniqueVerts * 3);
		texCoords.reserve((size_t)uniqueVerts * 2);
		normals.reserve((size_t)uniqueVerts * 3);
		indices.reserve(6 * ((size_t)vertices - 1) * ((size_t)vertices - 1));

		// Create the terrain positions and texture coords
		for (uint32_t i = 0; i < vertices; i++) {
			for (uint32_t j = 0; j < vertices; j++) {
				positions.push_back(((float)j / vertices - 1) * size);
				positions.push_back(50.0f * stb_perlin_noise3(10.0f * j / vertices, 0.0f, 10.0f * i / vertices, 0, 0, 0));
				positions.push_back(((float)i / vertices - 1) * size);
				texCoords.push_back(((float)j / vertices - 1) * texRepeats);
				texCoords.push_back(((float)i / vertices - 1) * texRepeats);
			}
		}

		// Now that we have positions, we can go back and calculate normals with a bit of math
		for (uint32_t i = 0; i < vertices; i++) {
			for (uint32_t j = 0; j < vertices; j++) {
				glm::vec3 normal = glm::vec3(2.0f);
				uint32_t leftIndex = 1 + 3 * (j - 1 + i * vertices);
				uint32_t rightIndex = 1 + 3 * (j + 1 + i * vertices);
				uint32_t downIndex = 1 + 3 * (j + (i - 1) * vertices);
				uint32_t upIndex = 1 + 3 * (j + (i + 1) * vertices);
				float heightL = 0.0f;
				float heightR = 0.0f;
				float heightD = 0.0f;
				float heightU = 0.0f;
				if (leftIndex >= 0 && leftIndex < positions.size()) {
					heightL = positions[leftIndex];
				}
				if (rightIndex >= 0 && rightIndex < positions.size()) {
					heightR = positions[rightIndex];
				}
				if (downIndex >= 0 && downIndex < positions.size()) {
					heightD = positions[downIndex];
				}
				if (upIndex >= 0 && upIndex < positions.size()) {
					heightU = positions[upIndex];
				}
				normal.x = heightL - heightR;
				normal.z = heightD - heightU;
				normal = glm::normalize(normal);
				normals.push_back(normal.x);
				normals.push_back(normal.y);
				normals.push_back(normal.z);
			}
		}

		// Construct the indices
		for (uint32_t z = 0; z < vertices - 1; z++) {
			for (uint32_t x = 0; x < vertices - 1; x++) {
				int topLeft = (z * vertices) + x;
				int topRight = topLeft + 1;
				int bottomLeft = ((z + 1) * vertices) + x;
				int bottomRight = bottomLeft + 1;
				indices.push_back(topLeft);
				indices.push_back(bottomLeft);
				indices.push_back(topRight);
				indices.push_back(topRight);
				indices.push_back(bottomLeft);
				indices.push_back(bottomRight);
			}
		}

		// Build the VAO from the data
		uint32_t vao = loader.loadToVAO(positions.data(), texCoords.data(), normals.data(), indices.data(), (uint32_t)indices.size(), uniqueVerts);
		Mesh terrainMesh;
		terrainMesh.vaoID = vao;
		terrainMesh.vertexCount = (uint32_t)indices.size();

		// For some reason C++ tends to hold on to this data
		positions.clear();
		texCoords.clear();
		normals.clear();
		indices.clear();

		return terrainMesh;
	}
}