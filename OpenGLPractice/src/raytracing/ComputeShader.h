#pragma once

#include <unordered_map>
#include "glm/glm.hpp"

namespace Renderer
{
	class ComputeShader
	{
	private:
		unsigned int m_RendererID;
		// caching for uniforms - 缓存统一变量的位置 - GLint
		mutable std::unordered_map<std::string, int> m_UniformLocationCache;

	public:
		ComputeShader() = default;
		ComputeShader(const std::string& computePath);
		~ComputeShader();

		void Bind() const;
		void Unbind() const;

		// #ToDO:have to modify
		inline unsigned int GetRendererID() const { return m_RendererID; }

		// Set uniforms
		void SetUniform1i(const std::string& name, int value);
		void SetUniform1f(const std::string& name, float value);
		void SetUniform1iv(const std::string& name, int count, int* value);
		void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
		void SetUniformVec2f(const std::string& name, const glm::vec2& vec);
		void SetUniformVec3f(const std::string& name, const glm::vec3& vec);
		void SetUniformVec4f(const std::string& name, const glm::vec4& vec);
		void SetUniformMat4f(const std::string& name, int count, const glm::mat4& matrix);

	private:
		int GetUniformLocation(const std::string& name) const;
		std::string ParseShader(const std::string& computePath);
		unsigned int CompileShader(unsigned int type, const std::string& source);
		unsigned int CreateShader(const std::string& computeShader);
	};
}