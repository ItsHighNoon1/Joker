#pragma once

#include <map>
#include <vector>

#include <glad/glad.h>
#include <OpenAL/al.h>

#include "render/Renderable.h"

namespace Joker {
	class Loader {
		// Handles all operations involving memory management
	public:
		Loader(); // For a handful of one-time setup things
		Mesh loadToVAO(GLfloat* positions, GLfloat* texCoords, GLfloat* normals, GLuint* indices, GLsizei count, GLsizei uniqueVertices);
		Mesh loadQuad();
		Mesh loadFromOBJ(const char* path);
		GLuint loadTexture(const char* path);
		Framebuffer loadFramebuffer(GLsizei width, GLsizei height);
		ALuint loadAudioBuffer(char* data, ALenum format, ALsizei size, ALsizei freq);
		ALuint loadFromWAV(const char* path);
		ALuint createSource();
		//std::map<uint8_t, TextChar> loadFont(const char* path);
		void cleanUp(); // Clear out all the memory we used
	private:
		std::vector<GLuint> vaos;
		std::vector<GLuint> vbos;
		std::vector<GLuint> textures;
		std::vector<GLuint> framebuffers;
		std::vector<ALuint> audioBuffers;
		std::vector<ALuint> audioSources;

		void storeDataInAttributeList(GLuint attributeNumber, GLfloat* data, GLsizeiptr totalSize, GLsizei vertexLength);
		void bindIndicesBuffer(GLuint* indices, GLsizeiptr size);
	};
}
