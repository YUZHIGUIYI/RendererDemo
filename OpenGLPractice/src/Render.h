#pragma once

#include <glad/glad.h>
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

// only MSVC works
#define ASSERT(x) if (!(x)) __debugbreak();

namespace Renderer
{
	void GLClearError();

	bool GLLogCall(const char* function, const char* file, int line);

	class Render
	{
	public:
		void Clear();
		void Draw(const Renderer::VertexArray& va, const Renderer::IndexBuffer& ib, 
			const Renderer::Shader& shader) const;
	};
}
