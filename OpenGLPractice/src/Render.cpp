#include "Render.h"
#include "Log.h"

namespace Renderer 
{
	void GLClearError()
	{
		while (glGetError() != GL_NO_ERROR);
	}

	bool GLLogCall(const char* function, const char* file, int line)
	{
		while (GLenum error = glGetError())
		{
			RD_CORE_ERROR("[OpenGL Error] ({0}) {1} {2}: {3}", error, function, file, line);
			return false;
		}
		return true;
	}

	void Render::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Render::Draw(const Renderer::VertexArray& va, const Renderer::IndexBuffer& ib, const Renderer::Shader& shader) const
	{
		shader.Bind();

		va.Bind();
		ib.Bind();

		glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, NULL);
	}

}
