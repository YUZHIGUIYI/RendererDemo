#include "GameLevel.h"
#include <fstream>
#include <sstream>

namespace breakout
{
	GameLevel::GameLevel()
	{
	}

	GameLevel::~GameLevel()
	{
	}

	void GameLevel::Load(const std::string& file, unsigned int levelWidth, unsigned int levelHeight)
	{
		// clear old data
		this->Bricks.clear();
		// load from file 
		unsigned int tileCode;
		//GameLevel level;  // ???
		std::string line;
		std::ifstream fstream(file);
		std::vector<std::vector<unsigned int>> tileData;
		if (fstream)
		{
			// read each line from level file
			while (std::getline(fstream, line))
			{
				std::istringstream sstream(line);
				std::vector<unsigned int> row;
				// read each word separated by spaces
				while (sstream >> tileCode)
				{
					row.push_back(tileCode);
				}
				tileData.push_back(row);
			}
			if (!tileData.empty())
			{
				init(tileData, levelWidth, levelHeight);
			}
		}
	}

	void GameLevel::Draw(SpriteRenderer& renderer)
	{
		for (auto& tile : Bricks)
		{
			if (!tile.Destroyed)
			{
				tile.Draw(renderer);
			}
		}
	}

	bool GameLevel::IsCompleted()
	{
		for (auto& tile : Bricks)
		{
			if (!tile.IsSolid && !tile.Destroyed)
			{
				return false;
			}
		}
		return true;
	}

	void GameLevel::init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, 
		unsigned int levelHeight)
	{
		// calculate dimension
		// TODO: Fix me
		unsigned int height = tileData.size();
		unsigned int width = tileData[0].size();
		float unit_width = levelWidth / static_cast<float>(width);
		float unit_height = levelHeight / static_cast<float>(height);
		// initialize level tiles based on tileData
		for (unsigned int y = 0; y < height; ++y)
		{
			for (unsigned int x = 0; x < width; ++x)
			{
				// check block type from data (20 level array)
				if (tileData[y][x] == 1)  // solid
				{
					glm::vec2 pos(unit_width * x, unit_height * y);
					glm::vec2 size(unit_width, unit_height);
					GameObject obj(pos, size, ResourceManager::GetTexture("block_solid"),
						glm::vec3(0.8f, 0.8f, 0.7f));
					obj.IsSolid = true;
					this->Bricks.push_back(obj);
				}
				else if (tileData[y][x] > 1)
				{
					glm::vec3 color = glm::vec3(1.0f);
					switch (tileData[y][x])
					{
					case 2:
						color = glm::vec3(0.2f, 0.6f, 1.0f);
						break;
					case 3:
						color = glm::vec3(0.0f, 0.7f, 0.0f);
						break;
					case 4:
						color = glm::vec3(0.8f, 0.8f, 0.4f);
						break;
					case 5:
						color = glm::vec3(1.0f, 0.5f, 0.0f);
						break;
					default:
						break;
					}
					
					glm::vec2 pos(unit_width * x, unit_height * y);
					glm::vec2 size(unit_width, unit_height);
					this->Bricks.push_back(GameObject(pos, size,
						ResourceManager::GetTexture("block"), color));
				}
			}
		}
	}

}