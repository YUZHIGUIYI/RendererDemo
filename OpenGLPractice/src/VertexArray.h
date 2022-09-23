#pragma once

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

namespace Renderer
{
	class VertexArray
	{
	private:
		unsigned int m_RendererID;

	public:
		VertexArray();
		~VertexArray();

		void AddBuffer(const Renderer::VertexBuffer& vb, const Renderer::VertexBufferLayout& layout);

		void Bind() const;
		void Unbind() const;
	};
}