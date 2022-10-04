#pragma once

#include "GameObject.h"

namespace breakout
{
	const glm::vec2 PowerUp_Size(60.0f, 20.0f);
	const glm::vec2 Velocity(0.0f, 150.0f);

	// PowerUp inherits its state and rendering functions from
	// GameObject but also holds extra information to state its
	// active duration and whether it is activated or not. 
	// The type of PowerUp is stored as a string.
	class PowerUp : public GameObject
	{
	public:
		// powerup state
		std::string Type;
		float Duration;
		bool Activated;
		// constructor
		PowerUp(std::string type, glm::vec3 color, float duration, glm::vec2 position, Renderer::Texture2D texture)
			: GameObject(position, PowerUp_Size, texture, color, Velocity), 
			Type(type), Duration(duration), Activated() { }
	};
}
