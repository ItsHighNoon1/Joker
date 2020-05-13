#define STB_PERLIN_IMPLEMENTATION
#include "Joker.h"

namespace Joker {
	// No body because all we need is to initialize allocator
	GeometryGenerator::GeometryGenerator(Allocator& allocator) : loader(allocator) {}

	Mesh GeometryGenerator::generateTerrain(float size, uint32_t vertices, uint32_t texRepeats) {
		// Set up variables that we will push data into
		uint32_t uniqueVerts = vertices * vertices;
		float* positions = new float[(uint64_t)uniqueVerts * 3];
		float* texCoords = new float[(uint64_t)uniqueVerts * 2];
		float* normals = new float[(uint64_t)uniqueVerts * 3];
		uint32_t* indices = new uint32_t[6 * ((size_t)vertices - 1) * ((size_t)vertices - 1)];

		// Create the terrain positions and texture coords
		uint32_t vertexPointer = 0;
		for (uint32_t i = 0; i < vertices; i++) {
			for (uint32_t j = 0; j < vertices; j++) {
				positions[3 * vertexPointer] = ((float)j / vertices - 1) * size;
				positions[3 * vertexPointer + 1] = 50.0f * stb_perlin_noise3(10.0f * j / vertices, 0.0f, 10.0f * i / vertices, 0, 0, 0);
				positions[3 * vertexPointer + 2] = ((float)i / vertices - 1) * size;
				texCoords[2 * vertexPointer] = ((float)j / vertices - 1) * texRepeats;
				texCoords[2 * vertexPointer + 1] = ((float)i / vertices - 1) * texRepeats;
				vertexPointer++;
			}
		}

		// Now that we have positions, we can go back and calculate normals with a bit of math
		vertexPointer = 0;
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
				if (leftIndex >= 0 && leftIndex < uniqueVerts * 3) {
					heightL = positions[leftIndex];
				}
				if (rightIndex >= 0 && rightIndex < uniqueVerts * 3) {
					heightR = positions[rightIndex];
				}
				if (downIndex >= 0 && downIndex < uniqueVerts * 3) {
					heightD = positions[downIndex];
				}
				if (upIndex >= 0 && upIndex < uniqueVerts * 3) {
					heightU = positions[upIndex];
				}
				normal.x = heightL - heightR;
				normal.z = heightD - heightU;
				normal = glm::normalize(normal);
				normals[3 * vertexPointer] = normal.x;
				normals[3 * vertexPointer + 1] = normal.y;
				normals[3 * vertexPointer + 2] = normal.z;
				vertexPointer++;
			}
		}

		// Construct the indices
		vertexPointer = 0;
		for (uint32_t z = 0; z < vertices - 1; z++) {
			for (uint32_t x = 0; x < vertices - 1; x++) {
				int topLeft = (z * vertices) + x;
				int topRight = topLeft + 1;
				int bottomLeft = ((z + 1) * vertices) + x;
				int bottomRight = bottomLeft + 1;
				indices[vertexPointer++] = topLeft;
				indices[vertexPointer++] = bottomLeft;
				indices[vertexPointer++] = topRight;
				indices[vertexPointer++] = topRight;
				indices[vertexPointer++] = bottomLeft;
				indices[vertexPointer++] = bottomRight;
			}
		}

		// Build the VAO from the data
		uint32_t vao = loader.loadToVAO(positions, texCoords, normals, indices, vertexPointer, uniqueVerts);
		Mesh terrainMesh;
		terrainMesh.vaoID = vao;
		terrainMesh.vertexCount = vertexPointer;

		// These arrays are heap allocated, so kill them
		delete[] positions;
		delete[] texCoords;
		delete[] normals;
		delete[] indices;

		return terrainMesh;
	}
}