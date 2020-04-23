#pragma once

#include <glad/glad.h>

namespace Joker {
	struct Mesh {
		// Essentially just a VAO, but we need to keep track of vertex count, plus it's more obvious than uint32_t
		GLuint vaoID;
		GLsizei vertexCount;
	};
}