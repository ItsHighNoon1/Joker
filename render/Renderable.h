#pragma once

#include <map>
#include <string>

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

	// A single character within a font
	struct FontChar {
		glm::vec2 position;
		glm::vec2 size;
		glm::vec2 offset;
		float xAdvance;
	};

	// An entire typeface
	struct Font {
		std::map<uint8_t, FontChar> data;
		uint32_t texture; // The map contains data about how to read the texture; we only need the GL id
	};

	// Entity instance
	struct StaticRenderable {
		Model model;
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
		int texIndex;
		glm::mat4 modelMatrix; // Wasteful to calculate more than once per frame
	};

	// Particle instance
	struct ParticleRenderable {
		Texture texture;
		glm::vec3 position;
		glm::vec2 scale;
		int texIndex;
		glm::mat4 modelMatrix;
	};

	// GUI instance
	struct GUIRenderable {
		Texture texture;
		glm::vec2 position;
		glm::vec2 scale;
		int texIndex;
		glm::mat4 transformationMatrix;
	};

	// Text instance
	struct TextRenderable {
		Font font;
		glm::vec2 position;
		glm::vec2 scale;
		std::string string;
		glm::vec3 color;
		glm::mat4 transformationMatrix;
	};
}