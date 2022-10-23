#pragma once

#include "Texture2D.h"
#include "SpriteRenderer.h"
#include "Shader.h"

namespace breakout
{
	// PostProcessor hosts all PostProcessing effects for the Breakout
	// Game. It renders the game on a textured quad after which one can
	// enable specific effects by enabling either the Confuse, Chaos or 
	// Shake boolean. 
	// It is required to call BeginRender() before rendering the game
	// and EndRender() after rendering the game for the class to work.
	class PostProcessor
	{
	public:
		// state 
		Renderer::Shader* PostProcessingShader;
		Renderer::Texture2D Texture;
		unsigned int Width, Height;
		// options
		bool Confuse, Chaos, Shake;
		// constructor
		PostProcessor(Renderer::Shader* shader, unsigned int width, unsigned int height);
		// prepares the postprocessor's framebuffer operations before rendering the game
		void BeginRender();
		// should be called after rendering the game, so it stores all the rendered data into a texture object
		void EndRender();
		// renders the PostProcessor texture quad (as a screen-encompassing large sprite)
		void Render(float time);

	private:
		// render state
		// MSFBO = Multisampled FBO. FBO is regular, used for blitting MS color-buffer to texture
		unsigned int MSFBO, FBO;
		// RBO is used for multisampled color buffer
		unsigned int RBO;

		unsigned int VAO;
		// initialize quad for rendering postprocessing texture
		void initRenderData();
	};
}