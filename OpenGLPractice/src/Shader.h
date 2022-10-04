#pragma once

#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace Renderer
{
	struct ShaderProgramSource
	{
		std::string VertexSource;
		std::string FragmentSource;
		std::string GeometrySource;
	};

	class Shader
	{
	private:
		std::string m_VertexPath;
		std::string m_FragmentPath;
		unsigned int m_RendererID;
		// caching for uniforms - 缓存统一变量的位置 - GLint
		mutable std::unordered_map<std::string, int> m_UniformLocationCache;
	public:
		Shader(const std::string& vertexPath, const std::string& fragmentPath, 
			const std::string& geometryPath = "");
		Shader() {}
		~Shader();

		void Bind() const;
		void Unbind() const;

		// Set uniforms
		void SetUniform1i(const std::string& name, int value);
		void SetUniform1f(const std::string& name, float value);
		void SetUniform1iv(const std::string& name, int count, int* value);
		void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
		void SetUniformVec2f(const std::string& name, const glm::vec2& vec);
		void SetUniformVec3f(const std::string& name, const glm::vec3& vec);
		void SetUniformVec4f(const std::string& name, const glm::vec4& vec);
		void SetUniformMat4f(const std::string& name, int count, const glm::mat4& matrix);

		// #ToDO:have to modify
		inline unsigned int GetRendererID() const { return m_RendererID; }

	private:
		int GetUniformLocation(const std::string& name) const;
		ShaderProgramSource ParseShader(const std::string& vertexPath, const std::string& fragmentPath,
			const std::string& geometryPath = "");
		unsigned int CompileShader(unsigned int type, const std::string& source);
		unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader,
			const std::string& geometryShader = "");

		void CheckProgramError(unsigned int ProgramId);
	};
}
