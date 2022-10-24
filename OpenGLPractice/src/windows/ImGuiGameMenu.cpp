#include "ImGuiGameMenu.h"
#include <imgui/imgui.h>
#include "Log.h"
#include "Input.h"


ImGuiGameMenu::ImGuiGameMenu()
	: m_Game(nullptr)
{
}

ImGuiGameMenu::~ImGuiGameMenu()
{
}

void ImGuiGameMenu::Update(const Camera& camera, RenderScene* scenebuffer)
{
	if (bshow)
	{
		if (!Begin("Breakout"))
		{
			End();
		}
		else
		{
			if (m_Game != nullptr)
			{
				m_Game->ProcessInput(Renderer::Input::DeltaTime);
				m_Game->Update(Renderer::Input::DeltaTime);
				m_Game->Render();
				scenebuffer->BlitFramebuffer();
			}
			else
			{
				if (ImGui::Button("Start Game"))
				{
					m_Game = std::make_unique<breakout::Game>(scenebuffer->GetWidth(), scenebuffer->GetHeight());
					m_Game->Init();
					Renderer::Input::RendererMode = Renderer::RENDERERMODE::GAME_MODE;
					RD_WARN("Start Game Break");
				}
			}
			if (ImGui::Button("Exit Game"))
			{
				if (m_Game != nullptr)
				{
					m_Game.reset();
					Renderer::Input::RendererMode = Renderer::RENDERERMODE::SAMPLE_MODE;
					RD_WARN("Exit Game Breakout");
				}
			}
			End();
		}
	}
}
