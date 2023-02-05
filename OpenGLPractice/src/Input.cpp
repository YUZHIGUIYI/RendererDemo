#include "Input.h"

namespace Renderer
{
	using double_ms = std::chrono::duration<double, std::milli>;

	std::chrono::steady_clock::time_point Input::s_StartTimePoint;

	std::unique_ptr<Renderer::Input> Input::s_Instance;

	Renderer::RENDERERMODE Input::RendererMode;

	bool Input::KeysProcessed[1024];

	bool Input::Keys[1024];

	bool Input::MouseButtons[5];

	float Input::DeltaTime = 0.0f;
	double Input::s_DeltaTime = 0.0;

	void Input::Init()
	{
		s_Instance = std::make_unique<Renderer::Input>();
		RendererMode = RENDERERMODE::SAMPLE_MODE;
	}

	void Input::Start()
	{
		s_StartTimePoint = std::chrono::high_resolution_clock::now();
	}

	void Input::Stop()
	{
		auto stop = std::chrono::high_resolution_clock::now();
		s_DeltaTime = std::chrono::duration_cast<double_ms>(stop - s_StartTimePoint).count();
	}

}