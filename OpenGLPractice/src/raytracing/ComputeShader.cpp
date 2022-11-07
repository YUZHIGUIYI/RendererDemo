#include "ComputeShader.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include "Log.h"
#include "glm/gtc/type_ptr.hpp"

namespace Renderer
{

	ComputeShader::ComputeShader(const std::string& computePath)
	{
		std::string source = ParseShader(computePath);
		m_RendererID = CreateShader(source);
	}

	ComputeShader::~ComputeShader()
	{
		glDeleteProgram(m_RendererID);
	}

	void ComputeShader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void ComputeShader::Unbind() const
	{
		glUseProgram(0);
	}

	void ComputeShader::SetUniform1i(const std::string& name, int value)
	{
		glUniform1i(GetUniformLocation(name), value);
	}

	void ComputeShader::SetUniform1f(const std::string& name, float value)
	{
		glUniform1f(GetUniformLocation(name), value);
	}

	void ComputeShader::SetUniform1iv(const std::string& name, int count, int* value)
	{
		glUniform1iv(GetUniformLocation(name), count, value);
	}

	void ComputeShader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
	{
		glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
	}

	void ComputeShader::SetUniformVec2f(const std::string& name, const glm::vec2& vec)
	{
		glUniform2f(GetUniformLocation(name), vec[0], vec[1]);
	}

	void ComputeShader::SetUniformVec3f(const std::string& name, const glm::vec3& vec)
	{
		glUniform3f(GetUniformLocation(name), vec[0], vec[1], vec[2]);
	}

	void ComputeShader::SetUniformVec4f(const std::string& name, const glm::vec4& vec)
	{
		glUniform4f(GetUniformLocation(name), vec[0], vec[1], vec[2], vec[3]);
	}

	void ComputeShader::SetUniformMat4f(const std::string& name, int count, const glm::mat4& matrix)
	{
		glUniformMatrix4fv(GetUniformLocation(name), count, GL_FALSE, glm::value_ptr(matrix));
	}

	int ComputeShader::GetUniformLocation(const std::string& name) const
	{
		if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
			return m_UniformLocationCache[name];

		int location = glGetUniformLocation(m_RendererID, name.c_str());
		// 未被使用的统一变量可能返回-1
		if (location == -1)
		{
			RD_WARN("Warning: uniform '{0}' doesn't exist!", name);
		}

		m_UniformLocationCache[name] = location;
		return location;
	}

	std::string ComputeShader::ParseShader(const std::string& computePath)
	{
		std::ifstream streamVertex(computePath);

		std::string line;
		std::stringstream ss;
		while (getline(streamVertex, line))
		{
			ss << line << "\n";
		}
		return ss.str();
	}

	unsigned int ComputeShader::CompileShader(unsigned int type, const std::string& source)
	{
		unsigned int id = glCreateShader(type);
		const char* src = source.c_str();
		glShaderSource(id, 1, &src, nullptr);
		glCompileShader(id);

		// 着色器编译错误处理
		int result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE)
		{
			int length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			char* message = (char*)malloc(length * sizeof(char));
			glGetShaderInfoLog(id, length, &length, message);
			RD_ERROR("Failed to compile {0}: {1}", "ComputeShader", message);
			glDeleteShader(id);
			free(message);
			return 0;
		}

		return id;
	}

	unsigned int ComputeShader::CreateShader(const std::string& computeShader)
	{
		unsigned int program = glCreateProgram();
		unsigned int cs = CompileShader(GL_COMPUTE_SHADER, computeShader);

		glAttachShader(program, cs);
		glLinkProgram(program);
		glValidateProgram(program);
		glDeleteShader(cs);

		return program;
	}

}