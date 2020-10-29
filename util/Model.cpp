#include "Model.h"

#include <iostream>
#include <vector>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Joker {
	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture> textures) {
		// Store data for later usage
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		// Allocate buffers
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		// C++ packs classes, so we can upload our struct directly
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

		// Specify layout data
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

		// Unbind VAO to protect it form random calls
		glBindVertexArray(0);
	}

	Mesh::~Mesh() {
		// Self-contained cleanup
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
		for (int i = 0; i < textures.size(); i++) {
			glDeleteTextures(1, &(textures[i].id));
		}
	}

	void Mesh::draw() {

	}

	Model::Model(std::string filename) {
		loadModel(filename);
	}

	void Model::draw() {
		for (int i = 0; i < meshes.size(); i++) {
			meshes[i].draw();
		}
	}

	void Model::loadModel(std::string filename) {
		// Import file
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filename,
			aiProcess_Triangulate |
			aiProcess_FlipUVs);

		// Make sure file is valid
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cerr << "Failed to load model " << filename << std::endl;
			return;
		}

		// Store directory of file for later use
		directory = filename.substr(0, filename.find_last_of('/'));

		// Recursive function to process assimp nodes
		processNode(scene->mRootNode, scene);
	}
	
	void Model::processNode(aiNode* node, const aiScene* scene) {
		// Process meshes owned by this node
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}

		// Process other child nodes
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
		// Initialize vectors
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<Texture> textures;

		// Iterate over vertices and add them to vector
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex;
			vertex.position.x = mesh->mVertices[i].x;
			vertex.position.y = mesh->mVertices[i].y;
			vertex.position.z = mesh->mVertices[i].z;
			// Model may not have tex coords
			if (mesh->mTextureCoords[0]) {
				vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
				vertex.texCoords.x = mesh->mTextureCoords[0][i].y;
			}
			vertex.normal.x = mesh->mNormals[i].x;
			vertex.normal.y = mesh->mNormals[i].y;
			vertex.normal.z = mesh->mNormals[i].z;
			vertices.push_back(vertex);
		}

		// Get indices
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}

		// Handle material
		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			std::vector<Texture> diffuseTextures = loadMaterialTextures(material, aiTextureType_DIFFUSE, Texture::Type::DIFFUSE);
			textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());
			std::vector<Texture> specularTextures = loadMaterialTextures(material, aiTextureType_SPECULAR, Texture::Type::SPECULAR);
			textures.insert(textures.end(), specularTextures.begin(), specularTextures.end());
		}

		return Mesh(vertices, indices, textures);
	}

	std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, Texture::Type typeName) {
		std::vector<Texture> textures;

		// Iterate over material textures
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
			aiString str;
			mat->GetTexture(type, i, &str);
			Texture texture;
			texture.id = loadTexture(str.C_Str(), directory);
			texture.type = typeName;
			textures.push_back(texture);
		}

		return textures;
	}

	uint32_t Model::loadTexture(const char* filename, const std::string& dir) {
		// Traditional texture load
		std::string fullPath = (dir + '/') + std::string(filename);

		// Generate texture
		uint32_t textureID;
		glGenTextures(1, &textureID);

		// stb
		int width;
		int height;
		int nrComponents;
		unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &nrComponents, 0);
		if (data) {
			// Determine format
			GLenum format;
			switch (nrComponents) {
			case 1: format = GL_RED; break;
			case 3: format = GL_RGB; break;
			default: format = GL_RGBA;
			}

			// Upload texture data
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			// Set texture params
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		} else {
			std::cout << "Texture load failed at " << fullPath << std::endl;
			stbi_image_free(data);
		}

		return textureID;
	}
}