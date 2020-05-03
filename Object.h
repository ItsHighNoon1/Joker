#pragma once

#include <map>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Joker {
	struct Mesh {
		// Essentially just a VAO, but we need to keep track of vertex count, plus it's more obvious than uint32_t
		GLuint vaoID;
		GLsizei vertexCount;
	};
	struct Model {
		// The most basic thing we can render
		Mesh mesh;
		GLuint texture;
	};
	struct Framebuffer {
		// A texture we can render to
		GLuint buffer;
		GLuint colorbuffer;
		GLuint depthbuffer;
		GLsizei width;
		GLsizei height;
	};
	struct TextChar {
		// A single character
		glm::vec2 position;
		glm::vec2 size;
		glm::vec2 offset;
		float xAdvance;
	};
	struct Text {
		// A string of text that we can render
		const char* string;
		std::map<uint8_t, TextChar> font; // That's a lot of memory to move
	};
}