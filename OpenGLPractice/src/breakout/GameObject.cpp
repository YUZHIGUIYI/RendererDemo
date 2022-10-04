#include "GameObject.h"

namespace breakout
{

	GameObject::GameObject()
		: Position(0.0f, 0.0f), Size(1.0f, 1.0f), Velocity(0.0f), Color(1.0f), Rotation(0.0f), 
		Sprite(), IsSolid(false), Destroyed(false)
	{

	}

	GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Renderer::Texture2D sprite, glm::vec3 color /*= glm::vec3(1.0f)*/, glm::vec2 velocity /*= glm::vec2(0.0f, 0.0f)*/)
		: Position(pos), Size(size), Velocity(velocity), Color(color), Rotation(0.0f), 
		Sprite(sprite), IsSolid(false), Destroyed(false)
	{

	}

	void GameObject::Draw(SpriteRenderer& renderer)
	{
		renderer.DrawSprite(Sprite, Position, Size, Rotation, Color);
	}

}