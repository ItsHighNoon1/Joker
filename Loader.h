#pragma once

#include <vector>

#include <glad/glad.h>

#include "Model.h"

namespace Joker {
	class Loader {
		// Handles all operations involving memory management
	public:
		Mesh loadToVAO(GLfloat* positions, GLuint* indices, GLsizei count);
		void cleanUp();
	private:
		std::vector<GLuint> vaos;
		std::vector<GLuint> vbos;

		GLuint createVAO();
		void storeDataInAttributeList(GLuint attributeNumber, GLfloat* data, GLsizeiptr totalSize, GLsizei vertexLength);
		void bindIndicesBuffer(GLuint* indices, GLsizeiptr size);
	};
}