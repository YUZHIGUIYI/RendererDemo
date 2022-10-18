#pragma once

#include <string>
#include "Camera.h"
#include "RenderScene.h"

class ImGuiWindowBase
{
public:
	ImGuiWindowBase();
	virtual ~ImGuiWindowBase() {};

	virtual void Update(const Camera& camera, RenderScene* scenebuffer);
	virtual void Show();

protected:
	virtual bool Begin(const std::string& name);
	void End();

	bool bshow;
};