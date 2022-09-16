#pragma once

#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
private:
	std::string m_VertexPath;
	std::string m_FragmentPath;
	unsigned int m_RenderID;
	// caching for uniforms - 缓存统一变量的位置 - GLint
	mutable std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	// Set uniforms
	void SetUniform1i(const std::string& name, int value);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform1iv(const std::string& name, int count, int* value);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformVec3f(const std::string& name, const glm::vec3& vec);
	void SetUniformVec4f(const std::string& name, const glm::vec4& vec);
	void SetUniformMat4f(const std::string& name, int count, const glm::mat4& matrix);
private:
	int GetUniformLocation(const std::string& name) const;
	ShaderProgramSource ParseShader(const std::string& vertexPath, const std::string& fragmentPath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

	void CheckProgramError(unsigned int ProgramId);
};