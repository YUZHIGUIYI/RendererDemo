#pragma once

#include "GameObject.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"
#include <vector>

namespace breakout
{
	// Game Level holds all tiles as part of a Breakout level and 
	// hosts functionality to load/render levels from harddisk
	class GameLevel
	{
	public:
		// level state
		std::vector<GameObject> Bricks;
		// constructor
		GameLevel() {}
		// loads level from file
		void Load(const std::string& file, unsigned int levelWidth, unsigned int levelHeight);
		// render level
		void Draw(SpriteRenderer& renderer);
		// check if the level is completed (all non-solid tiles are destroyed)
		bool IsCompleted();

	private:
		// initialize level from tile data
		void init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth,
			unsigned int levelHeight);
	};
}