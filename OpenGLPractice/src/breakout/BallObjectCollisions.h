#pragma once

#include "GameObject.h"
#include "Texture2D.h"

namespace breakout
{
	// BallObject holds the state of the Ball object inheriting
	// relevant state data from GameObject. Contains some extra
	// functionality specific to Breakout's ball object that
	// were too specific for within GameObject alone.
	class BallObject : public GameObject
	{
	public:
		// ball state
		float Radius;
		bool Stuck;
		bool Sticky, PassThrough;
		
		BallObject();
		BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Renderer::Texture2D sprite);
		// moves the ball, keeping it constrained within the window bounds (except bottom edge);
		// returns new position
		glm::vec2 Move(float dt, unsigned int window_width);
		// resets the ball to original state with given position and velocity
		void Reset(glm::vec2 position, glm::vec2 velocity);
	};
}