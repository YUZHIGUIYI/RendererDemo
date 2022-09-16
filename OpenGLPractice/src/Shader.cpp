#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
	: m_VertexPath(vertexPath), m_FragmentPath(fragmentPath)
{
	ShaderProgramSource source = ParseShader(vertexPath, fragmentPath);
	m_RenderID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
	// ɾ����ɫ����ǵĻ�����
	glDeleteProgram(m_RenderID);
}

void Shader::Bind() const
{
	glUseProgram(m_RenderID);
}

void Shader::Unbind() const
{
	glUseProgram(0);
}

void Shader::SetUniform1i(const std::string& name, int value)
{
	// ������ʹ�� - ������Ϊ����
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform1f(const std::string& name, float value)
{
	glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetUniform1iv(const std::string& name, int count, int* value)
{
	// ����������һ������
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

	int location = glGetUniformLocation(m_RenderID, name.c_str());
	// δ��ʹ�õ�ͳһ�������ܷ���-1
	if (location == -1)
	{
		std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
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

	return { ss[0].str(), ss[1].str() };  // stringstreamת��Ϊstring
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	// ��ɫ�����������
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)malloc(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
			<< std::endl;
		std::cout << message << std::endl;
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

	glDeleteShader(vs);  // ����ɾ��Դ��
	glDeleteShader(fs);  // Detach����ɾ���������Ƽ��������ڵ���

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
		std::cout << "Failed to link " << "Program" << std::endl;
		std::cout << message << std::endl;
		glDeleteProgram(ProgramId);
		free(message);
	}
}

