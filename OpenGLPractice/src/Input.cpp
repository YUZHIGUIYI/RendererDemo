#include "Input.h"

namespace Renderer
{

	std::unique_ptr<Renderer::Input> Input::s_Instance;

	Renderer::RENDERERMODE Input::RendererMode;

	bool Input::KeysProcessed[1024];

	float Input::DeltaTime = 0.0f;

	bool Input::Keys[1024];

	void Input::Init()
	{
		s_Instance = std::make_unique<Renderer::Input>();
		RendererMode = RENDERERMODE::SAMPLE_MODE;
	}
}