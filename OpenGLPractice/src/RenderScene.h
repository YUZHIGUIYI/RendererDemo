#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class RenderScene
{
public:
	RenderScene(unsigned int width, unsigned int height);
	~RenderScene();

	unsigned int GetFrameTexture();
	unsigned int GetFrameBuffer();

	void RescaleFrameBuffer(unsigned int width, unsigned int height);

	void Bind() const;
	void Unbind() const;
	void BlitFramebuffer() const;

	unsigned int GetWidth() const { return m_Width; }
	unsigned int GetHeight() const { return m_Height; }
	bool HasChanged() const
	{
		bool result = (m_PreWidth != m_Width && m_PreHeight != m_Height);
		m_PreWidth = m_Width;
		m_PreHeight = m_Height;
		return result;
	}

private:
	mutable unsigned int m_PreWidth;
	mutable unsigned int m_PreHeight;
	unsigned int m_Width, m_Height;
	unsigned int FBO;
	unsigned int TextureID;
	unsigned int RBO;
};