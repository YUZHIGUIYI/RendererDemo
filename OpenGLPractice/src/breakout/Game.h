#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "SpriteRenderer.h"
#include "GameObject.h"
#include "BallObjectCollisions.h"
#include "ParticleGenerator.h"
#include "PostProcessor.h"
#include "PowerUp.h"
#include "TextRenderer.h"
#include "GameLevel.h"

#include <irrKlang/irrKlang.h>

namespace breakout
{
	// Represents the current state of the game
	enum class GameState
	{
		GMAE_ACTIVE, GAME_MENU, GAME_WIN
	};
	
	// Represents the four possible (collision) directions
	enum class Direction
	{
		UP, RIGHT, DOWN, LEFT
	};

	// Define a Collision struct that represents collision data
	struct Collision
	{
		bool hit;
		Direction direction;
		glm::vec2 difference;
	};

	// Game holds all game-related state and functionality
	class Game
	{
	public:
		// Game state
		GameState State;
		bool Keys[1024]; // Also see RenderWindow KeyCallback function
		bool KeysProcessed[1024];
		unsigned int Width, Height;

		unsigned int Lives;  // player lives

		std::vector<GameLevel> Levels;
		unsigned int Level;

		std::vector<PowerUp> PowerUps;
		
	public:
		Game(unsigned int width, unsigned int height);
		~Game();

		// Initialize game state (load all shaders/textures/levels)
		void Init();
		// Game loop
		void ProcessInput(float dt);
		void Update(float dt);
		void Render();

		void DoCollisions();

		// reset game
		void ResetLevel();
		void ResetPlayer();

		// powerup
		bool ShouldSpawn(unsigned int chance);
		void SpawnPowerUps(GameObject& block);
		void UpdatePowerUps(float dt);
		void ActivatePowerUp(PowerUp& powerup);
		bool IsOtherPowerUpActive(const std::string& type);

	private:
		// AABB Collision
		bool CheckCollision(GameObject& one, GameObject& two);
		Collision CheckCollision(BallObject& one, GameObject& two);

		Direction VectorDirection(glm::vec2 target);

	private:
		irrklang::ISoundEngine* SoundEngine;
		static std::unique_ptr<SpriteRenderer> Renderer;
		static std::unique_ptr<GameObject> Player;
		static std::unique_ptr<BallObject> Ball;
		static std::unique_ptr<ParticleGenerator> Particles;
		static std::unique_ptr<PostProcessor> Effects;
		static std::unique_ptr<TextRenderer> Text;

		static glm::vec2 Player_Size;
		static float Player_Velocity;

		static glm::vec2 Initial_Ball_Velocity;
		static float Ball_Radius;

		static float ShakeTime;
	};
}