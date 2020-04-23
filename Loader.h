#pragma once

#include <vector>

#include <glad/glad.h>

#include "Model.h"

namespace Joker {
	class Loader {
		// Handles all operations involving memory management
	public:
		Mesh loadToVAO(float positions[], int indices[], int count);
		void cleanUp();
	private:
		std::vector<GLuint> vaos;
		std::vector<GLuint> vbos;

		GLuint createVAO();
		void storeDataInAttributeList(GLuint attributeNumber, float data[], GLsizei size);
		void bindIndicesBuffer(int indices[]);
	};
}