#include "Texture.h"
#include <glad/glad.h>
#include "stb_image/stb_image.h"
#include "Log.h"

namespace Renderer
{
	Texture::Texture(const std::string& path)
		: m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr),
		m_Width(0), m_Height(0), m_nrComponents(0)
	{
		// 纹理沿y轴上下翻转，因为纹理自顶向下扫描，而OpenGL从左下角开始
		stbi_set_flip_vertically_on_load(true);
		m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_nrComponents, 0);
		if (m_LocalBuffer)
		{
			GLenum format;
			if (m_nrComponents == 1) format = GL_RED;
			else if (m_nrComponents == 3) format = GL_RGB;
			else if (m_nrComponents == 4) format = GL_RGBA;

			glGenTextures(1, &m_RendererID);
			glBindTexture(GL_TEXTURE_2D, m_RendererID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, m_LocalBuffer);
			glGenerateMipmap(GL_TEXTURE_2D);
			// 当使用alpha纹理时，请将纹理环绕方式设置为GL_REPEAT -> GL_CLAMP_TO_EDGE
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			stbi_image_free(m_LocalBuffer);
		}
		else
		{
			RD_ERROR("Texure failed to load at path: {0}", path);
		}
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void Texture::Bind(unsigned int slot /*= 0*/) const
	{
		// 激活纹理插槽，绑定纹理单元
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}

	void Texture::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
