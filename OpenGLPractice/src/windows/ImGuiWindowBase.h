#pragma once

#include <string>
#include "Camera.h"

class ImGuiWindowBase
{
public:
	ImGuiWindowBase();
	virtual ~ImGuiWindowBase() {};

	virtual void Update(const Camera& camera);
	virtual void Show();

protected:
	virtual bool Begin(const std::string& name);
	void End();

	bool bshow;
};