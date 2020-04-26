#pragma once

#include <vector>

#include <glad/glad.h>

#include "Model.h"

namespace Joker {
	class Loader {
		// Handles all operations involving memory management
	public:
		Loader(); // For one-time setup
		Mesh loadToVAO(GLfloat* positions, GLfloat* texCoords, GLuint* indices, GLsizei count);
		GLuint loadTexture(const char* path);
		void cleanUp();
	private:
		std::vector<GLuint> vaos;
		std::vector<GLuint> vbos;
		std::vector<GLuint> textures;

		GLuint createVAO();
		void storeDataInAttributeList(GLuint attributeNumber, GLfloat* data, GLsizeiptr totalSize, GLsizei vertexLength);
		void bindIndicesBuffer(GLuint* indices, GLsizeiptr size);
	};
}