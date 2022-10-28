#pragma once

#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <AssimpGLMHelpers.h>
#include <Animdata.h>
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
	std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;
	int m_BoneCounter = 0;

	// TODO: have to fix
	Texture m_FirstTexture;
	// model data
	std::vector<Mesh> meshes;
	std::string directory;
	//std::vector<Texture> textures_loaded;
	std::unordered_map<std::string, Texture> textures_map;
	bool gammaCorrection;

public:
	auto& GetBoneInfoMap() { return m_BoneInfoMap; }
	int& GetBoneCount() { return m_BoneCounter; }
	
private:
	void SetVertexBoneDataToDefault(Vertex& vertex);
	void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
	void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);
	unsigned int TextureFromFile(const std::string& path, const std::string& directory, bool gamma = false);
};