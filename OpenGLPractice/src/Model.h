#pragma once

#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <unordered_map>

class Model
{
public:
	Model(const std::string& path, bool gamma = false);
	~Model();

	void Draw(Renderer::Shader& shader);

	inline std::vector<Mesh> GetMeshes() const { return meshes; }

	inline unsigned int GetMeshesSize() const { return meshes.size(); }

	inline unsigned int GetFirstTextureId() const { return m_FirstTexture.id; }

private:
	// ToDo: have to fix
	Texture m_FirstTexture;
	// model data
	std::vector<Mesh> meshes;
	std::string directory;
	//std::vector<Texture> textures_loaded;
	std::unordered_map<std::string, Texture> textures_map;
	bool gammaCorrection;

	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);
	unsigned int TextureFromFile(const std::string& path, const std::string& directory, bool gamma = false);
};