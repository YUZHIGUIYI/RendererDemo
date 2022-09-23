#include "Shader.h"
#include "Log.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace Renderer
{
	Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
		: m_VertexPath(vertexPath), m_FragmentPath(fragmentPath)
	{
		ShaderProgramSource source = ParseShader(vertexPath, fragmentPath);
		m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
	}

	Shader::~Shader()
	{
		// 删除着色器标记的缓冲区
		glDeleteProgram(m_RendererID);
	}

	void Shader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void Shader::Unbind() const
	{
		glUseProgram(0);
	}

	void Shader::SetUniform1i(const std::string& name, int value)
	{
		// 仅纹理使用 - 纹理插槽为整数
		glUniform1i(GetUniformLocation(name), value);
	}

	void Shader::SetUniform1f(const std::string& name, float value)
	{
		glUniform1f(GetUniformLocation(name), value);
	}

	void Shader::SetUniform1iv(const std::string& name, int count, int* value)
	{
		// 多个纹理组成一个纹理
		glUniform1iv(GetUniformLocation(name), count, value);
	}

	void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
	{
		glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
	}

	void Shader::SetUniformVec3f(const std::string& name, const glm::vec3& vec)
	{
		glUniform3f(GetUniformLocation(name), vec[0], vec[1], vec[2]);
	}

	void Shader::SetUniformVec4f(const std::string& name, const glm::vec4& vec)
	{
		glUniform4f(GetUniformLocation(name), vec[0], vec[1], vec[2], vec[3]);
	}

	void Shader::SetUniformMat4f(const std::string& name, int count, const glm::mat4& matrix)
	{
		glUniformMatrix4fv(GetUniformLocation(name), count, GL_FALSE, glm::value_ptr(matrix));
	}

	int Shader::GetUniformLocation(const std::string& name) const
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

	ShaderProgramSource Shader::ParseShader(const std::string& vertexPath, const std::string& fragmentPath)
	{
		std::ifstream streamVertex(vertexPath);
		std::ifstream streamFragment(fragmentPath);

		std::string line;
		std::stringstream ss[2];
		while (getline(streamVertex, line))
		{
			ss[0] << line << "\n";
		}
		while (getline(streamFragment, line))
		{
			ss[1] << line << "\n";
		}

		return { ss[0].str(), ss[1].str() };  // stringstream转换为string
	}

	unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
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
			RD_ERROR("Failed to compile {0}: {1}", (type == GL_VERTEX_SHADER ? "vertex" : "fragment"), message);
			glDeleteShader(id);
			free(message);
			return 0;
		}

		return id;
	}

	unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
	{
		unsigned int program = glCreateProgram();
		unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
		unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);
		glValidateProgram(program);

		glDeleteShader(vs);  // 并非删除源码
		glDeleteShader(fs);  // Detach才是删除，但不推荐，不利于调试

		return program;
	}

	void Shader::CheckProgramError(unsigned int ProgramId)
	{
		int result;
		glGetProgramiv(ProgramId, GL_LINK_STATUS, &result);
		if (result == GL_FALSE)
		{
			int length;
			glGetProgramiv(ProgramId, GL_INFO_LOG_LENGTH, &length);
			char* message = (char*)malloc(length * sizeof(char));
			glGetProgramInfoLog(ProgramId, length, &length, message);
			RD_ERROR("Failed to link Program: {0}", message);
			glDeleteProgram(ProgramId);
			free(message);
		}
	}
}


