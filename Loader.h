#pragma once

#include <vector>

#include <glad/glad.h>

#include "Model.h"

namespace Joker {
	class Loader {
		// Handles all operations involving memory management
	public:
		Loader(); // For a handful of one-time setup things
		Mesh loadToVAO(GLfloat* positions, GLfloat* texCoords, GLfloat* normals, GLuint* indices, GLsizei count);
		Mesh loadFromOBJ(const char* path);
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