#pragma once

#include <vector>

namespace Joker {
	class Allocator {
		// Handles creation and cleanup of OpenGL / OpenAL objects
	public:
		Allocator();
		~Allocator();

		uint32_t loadToVAO(float* positions, float* texCoords, float* normals, uint32_t* indices, uint32_t count, uint32_t uniqueVertices);
		uint32_t loadQuad();
		uint32_t loadTexture(const char* path);
		uint32_t loadFramebuffer(uint32_t width, uint32_t height, uint32_t* colorBuffer, uint32_t* depthBuffer);
		uint32_t loadAudioBuffer(char* data, uint32_t format, uint32_t size, uint32_t freq);
		uint32_t createSource();
	private:
		std::vector<uint32_t> vaos;
		std::vector<uint32_t> vbos;
		std::vector<uint32_t> textures;
		std::vector<uint32_t> framebuffers;
		std::vector<uint32_t> audioBuffers;
		std::vector<uint32_t> audioSources;

		void storeDataInAttributeList(uint32_t attributeNumber, float* data, uint32_t totalSize, uint32_t vertexLength);
		void bindIndicesBuffer(uint32_t* indices, uint32_t size);
	};
}