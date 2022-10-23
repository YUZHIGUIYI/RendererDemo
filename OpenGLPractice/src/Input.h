#pragma once

#include <memory>

namespace Renderer
{
	enum class RENDERERMODE
	{
		SAMPLE_MODE, RENDER_MODE, GAME_MODE,
	};

	class Input
	{
	private:
		static std::unique_ptr<Renderer::Input> s_Instance;

	public:
		static RENDERERMODE RendererMode;
		static bool Keys[1024];
		static bool KeysProcessed[1024];
		static float DeltaTime;
		
	public:
		static void Init();
	};
}
