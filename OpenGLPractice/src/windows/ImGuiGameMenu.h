#pragma once

#include "ImGuiWindowBase.h"
#include "breakout/Game.h"
#include <memory>

class ImGuiGameMenu : public ImGuiWindowBase
{
public:
	ImGuiGameMenu();
	~ImGuiGameMenu();

	void Update(const Camera& camera, RenderScene* scenebuffer) override;

private:
	std::unique_ptr<breakout::Game> m_Game;
};