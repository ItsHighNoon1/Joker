#pragma once

#include <glm/glm.hpp>

namespace Joker {
	// A VAO, but we keep track of the vertex count
	struct Mesh {
		uint32_t vaoID;
		uint32_t vertexCount;
	};

	// A texture, along with some extra data
	struct Texture {
		uint32_t texture;
		uint32_t numRows = 1; // 1 should be default
	};

	// Something we can render; every visible entity needs one
	struct Model {
		Mesh mesh;
		Texture texture;
	};

	// A framebuffer to render to
	struct Framebuffer {
		uint32_t width;
		uint32_t height;
		uint32_t buffer;
		uint32_t color;
		uint32_t depth;
	};

	// Entity instance
	struct StaticRenderable {
		Model model;
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
		glm::mat4 modelMatrix; // Wasteful to calculate more than once per frame
		int texIndex;
	};

	// Particle instance
	struct ParticleRenderable {
		Texture texture;
		glm::vec3 position;
		glm::vec2 scale;
		glm::mat4 modelMatrix;
		int texIndex;
	};

	// GUI instance
	struct GUIRenderable {
		Texture texture;
		glm::vec2 position;
		glm::vec2 scale;
		glm::mat4 transformationMatrix;
		int texIndex;
	};
}