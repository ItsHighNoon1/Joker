#pragma once

#include <vector>

#include <glad/glad.h>
#include <AL/al.h>

#include "Model.h"

namespace Joker {
	class Loader {
		// Handles all operations involving memory management
	public:
		Loader(); // For a handful of one-time setup things
		Mesh loadToVAO(GLfloat* positions, GLfloat* texCoords, GLfloat* normals, GLuint* indices, GLsizei count, GLsizei uniqueVertices);
		Mesh loadFromOBJ(const char* path);
		GLuint loadTexture(const char* path);
		ALuint loadAudioBuffer(char* data, ALenum format, ALsizei size, ALsizei freq);
		ALuint loadFromWAV(const char* path);
		ALuint createSource();
		void cleanUp(); // Clear out all the memory we used
	private:
		std::vector<GLuint> vaos;
		std::vector<GLuint> vbos;
		std::vector<GLuint> textures;
		std::vector<ALuint> audioBuffers;
		std::vector<ALuint> audioSources;

		void storeDataInAttributeList(GLuint attributeNumber, GLfloat* data, GLsizeiptr totalSize, GLsizei vertexLength);
		void bindIndicesBuffer(GLuint* indices, GLsizeiptr size);
	};
}