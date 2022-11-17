#pragma once

#include <memory>
#include <chrono>

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
		static std::chrono::steady_clock::time_point s_StartTimePoint;

	public:
		static RENDERERMODE RendererMode;
		static bool Keys[1024];
		static bool KeysProcessed[1024];
		static bool MouseButtons[5];
		static float DeltaTime;
		static double s_DeltaTime;
		
	public:
		static void Init();

		static void Start();
		static void Stop();
	};
}
