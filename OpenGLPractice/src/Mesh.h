#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Shader.h>
#include <vector>
#include <string>

#define MAX_BONE_INFLUENCE 4
#define MAX_BONE_WEIGHTS 100

struct Vertex
{
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texture coordinates
	glm::vec2 TexCoords;
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;
	// bone indexes which will influence the vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	// weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture
{
	// texture id
	// texture type - diffuse or specular or normal, etc
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh
{
private:
	// mesh data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	// render data
	unsigned int VAO, VBO, IBO;
public:
	Mesh(const std::vector<Vertex>& verticesIn, const std::vector<unsigned int>& indicesIn,
		const std::vector<Texture>& texturesIn);
	~Mesh();

	void Draw(Renderer::Shader& shader);

	void FreeMeshBuffer();

	inline unsigned int GetVertexArray() const { return VAO; }

	inline unsigned int GetIndicesSize() const { return static_cast<unsigned int>(indices.size()); }

	inline std::vector<Vertex> GetVertices() const { return vertices; }

private:

	void setupMesh();
};