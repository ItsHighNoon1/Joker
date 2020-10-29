#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

namespace Joker {
	// Helper structs
	struct Vertex {
		glm::vec3 position;
		glm::vec2 texCoords;
		glm::vec3 normal;
	};

	struct Texture {
		enum class Type {
			DIFFUSE, SPECULAR
		};

		uint32_t id;
		Type type;
	};

	class Mesh {
	public:
		Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture> textures);
		~Mesh();
		void draw();

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<Texture> textures;
	private:
		uint32_t vao;
		uint32_t vbo;
		uint32_t ebo;
	};

	class Model {
		// Model loaded from file system with associated textures
	public:
		Model(std::string filename);
		void draw();
	private:
		void loadModel(std::string filename);
		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, Texture::Type typeName);
		uint32_t loadTexture(const char* filename, const std::string& dir);

		std::vector<Mesh> meshes;
		std::string directory;
	};
}