#include "Game.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace breakout
{

	Game::Game(unsigned int width, unsigned int height)
		: State(GameState::GMAE_ACTIVE), Keys(), Width(width), Height(height)
	{
		SoundEngine = irrklang::createIrrKlangDevice();
	}

	Game::~Game()
	{
		SoundEngine->drop();
	}

	void Game::Init()
	{
		// load shaders
		ResourceManager::LoadShader("res/shaders/breakout/Sprite.vert",
			"res/shaders/breakout/Sprite.frag", "", "sprite");
		ResourceManager::LoadShader("res/shaders/breakout/Particle.vert",
			"res/shaders/breakout/Particle.frag", "", "particle");
		ResourceManager::LoadShader("res/shaders/breakout/PostProcessing.vert",
			"res/shaders/breakout/PostProcessing.frag", "", "postprocessing");
		// configure shaders
		glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
			static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
		ResourceManager::Shaders["sprite"].Bind();
		ResourceManager::Shaders["sprite"].SetUniform1i("image", 0);
		ResourceManager::Shaders["sprite"].SetUniformMat4f("projection", 1, projection);

		ResourceManager::Shaders["particle"].Bind();
		ResourceManager::Shaders["particle"].SetUniform1i("sprite", 0);
		ResourceManager::Shaders["particle"].SetUniformMat4f("projection", 1, projection);
		
		// load textures
		ResourceManager::LoadTexture("res/textures/background.jpg", false, "background");
		ResourceManager::LoadTexture("res/textures/awesomeface.png", true, "face");
		ResourceManager::LoadTexture("res/textures/block.png", false, "block");
		ResourceManager::LoadTexture("res/textures/block_solid.png", false, "block_solid");
		ResourceManager::LoadTexture("res/textures/paddle.png", true, "paddle");
		ResourceManager::LoadTexture("res/textures/particle.png", true, "particle");

		ResourceManager::LoadTexture("res/textures/powerup_speed.png", true, "powerup_speed");
		ResourceManager::LoadTexture("res/textures/powerup_sticky.png", true, "powerup_sticky");
		ResourceManager::LoadTexture("res/textures/powerup_increase.png", true, "powerup_increase");
		ResourceManager::LoadTexture("res/textures/powerup_confuse.png", true, "powerup_confuse");
		ResourceManager::LoadTexture("res/textures/powerup_chaos.png", true, "powerup_chaos");
		ResourceManager::LoadTexture("res/textures/powerup_passthrough.png", true, "powerup_passthrough");

		// set render-specific controls
		Renderer = std::make_unique<breakout::SpriteRenderer>(ResourceManager::Shaders["sprite"]);
		Particles = std::make_unique<breakout::ParticleGenerator>(ResourceManager::Shaders["particle"],
			ResourceManager::Textures["particle"], 500);
		Effects = std::make_unique<breakout::PostProcessor>(ResourceManager::Shaders["postprocessing"],
			this->Width, this->Height);
		Text = std::make_unique<breakout::TextRenderer>(this->Width, this->Height);
		Text->Load("res/fonts/ocraext.TTF", 24);

		// load levels
		GameLevel one;
		one.Load("res/levels/one.lvl", this->Width, this->Height / 2);
		GameLevel two;
		two.Load("res/levels/two.lvl", this->Width, this->Height / 2);
		GameLevel three;
		three.Load("res/levels/three.lvl", this->Width, this->Height / 2);
		GameLevel four;
		four.Load("res/levels/four.lvl", this->Width, this->Height / 2);

		this->Levels.reserve(4);
		this->Levels.push_back(one);
		this->Levels.push_back(two);
		this->Levels.push_back(three);
		this->Levels.push_back(four);
		this->Level = 0;
		
		// configure game objects - player
		glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - Player_Size.x / 2.0f, this->Height - Player_Size.y);
		Player = std::make_unique<breakout::GameObject>(playerPos, Player_Size, 
			ResourceManager::Textures["paddle"]);

		// configure ball
		glm::vec2 ballPos = playerPos + glm::vec2(Player_Size.x / 2.0f - Ball_Radius, -Ball_Radius * 2.0f);
		Ball = std::make_unique<BallObject>(ballPos, Ball_Radius, Initial_Ball_Velocity,
			ResourceManager::Textures["face"]);

		// audio
		SoundEngine->play2D("res/audio/breakout.mp3", true);
	}

	void Game::ProcessInput(float dt)
	{
		// select menu
		if (this->State == GameState::GAME_MENU)
		{
			if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])
			{
				this->State = GameState::GMAE_ACTIVE;
				this->KeysProcessed[GLFW_KEY_ENTER] = true;
			}
			if (this->Keys[GLFW_KEY_W] && !this->KeysProcessed[GLFW_KEY_W])
			{
				this->Level = (this->Level + 1) % 4;
				this->KeysProcessed[GLFW_KEY_W] = true;
			}
			if (this->Keys[GLFW_KEY_S] && !this->KeysProcessed[GLFW_KEY_S])
			{
				if (this->Level > 0)
					--this->Level;
				else
					this->Level = 3;
				this->KeysProcessed[GLFW_KEY_S] = true;
			}
		}
		// game active
		if (this->State == GameState::GMAE_ACTIVE)
		{
			float velocity = Player_Velocity * dt;
			// move player-board
			if (this->Keys[GLFW_KEY_A])
			{
				if (Player->Position.x >= 0.0f)
				{
					Player->Position.x -= velocity;
					if (Ball->Stuck) Ball->Position.x -= velocity;
				}
			}
			if (this->Keys[GLFW_KEY_D])
			{
				if (Player->Position.x <= this->Width - Player->Size.x)
				{
					Player->Position.x += velocity;
					if (Ball->Stuck) Ball->Position.x += velocity;
				}
			}
			if (this->Keys[GLFW_KEY_SPACE])
			{
				Ball->Stuck = false;
			}
		}
		// game win
		if (this->State == GameState::GAME_WIN)
		{
			if (this->Keys[GLFW_KEY_ENTER])
			{
				this->KeysProcessed[GLFW_KEY_ENTER] = true;
				Effects->Chaos = false;
				this->State = GameState::GAME_MENU;
			}
		}
	}

	void Game::Update(float dt)
	{
		// update the position of the ball each frame
		Ball->Move(dt, Width);

		// check for collisions
		DoCollisions();

		// update particles
		Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2.0f));

		// update PowerUps
		UpdatePowerUps(dt);

		// reduce shake time
		if (ShakeTime > 0.0f)
		{
			ShakeTime -= dt;
			if (ShakeTime <= 0.0f)
				Effects->Shake = false;
		}

		// TODO: Fix me
		// check loss condition - did ball reach bottom edge?
		if (Ball->Position.y >= Height)
		{
			--this->Lives;
			// did the player lose all his lives? Game over
			if (this->Lives == 0)
			{
				ResetLevel();
				this->State = GameState::GAME_MENU;
			}
			ResetPlayer();
		}

		// win
		if (this->State == GameState::GMAE_ACTIVE && this->Levels[this->Level].IsCompleted())
		{
			this->ResetLevel();
			this->ResetPlayer();
			Effects->Chaos = true;
			this->State = GameState::GAME_WIN;
		}
	}

	void Game::Render()
	{
		if (this->State == GameState::GMAE_ACTIVE || this->State == GameState::GAME_MENU
			|| this->State == GameState::GAME_WIN)
		{
			// begin rendering to postprocessing framebuffer
			Effects->BeginRender();
			// draw background
			// TODO: Fix me
			Renderer->DrawSprite(ResourceManager::Textures["background"], glm::vec2(0.0f, 0.0f),
				glm::vec2(this->Width, this->Height), 0.0f);
			// draw level
			this->Levels.at(this->Level).Draw(*Renderer);
			// draw player
			Player->Draw(*Renderer);
			// draw PowerUps
			for (auto& powerup : PowerUps)
			{
				if (!powerup.Destroyed) powerup.Draw(*Renderer);
			}
			// draw particles
			Particles->Draw();
			// draw ball
			Ball->Draw(*Renderer);
			// end rendering to postprocessing framebuffer
			Effects->EndRender();
			// render postprocessing quad
			Effects->Render(glfwGetTime());

			// render text (don't include in postprocessing)
			Text->RenderText("Lives:" + std::to_string(this->Lives), 5.0f, 5.0f, 1.0f);
		}
		if (this->State == GameState::GAME_MENU)
		{
			Text->RenderText("Press Enter key to start", 250.0f, Height / 2.0f, 1.0f);
			Text->RenderText("Press W or S key to select level", 245.0f, Height / 2.0f + 20.0f, 0.75f);
		}
		if (this->State == GameState::GAME_WIN)
		{
			Text->RenderText("You Win!!!", 320.0f, Height / 2.0f - 20.0f, 1.0f,
				glm::vec3(0.0, 1.0, 0.0));
			Text->RenderText("Press Enter key to retry or Esc key to quit",
				130.0f, Height / 2.0f, 1.0, glm::vec3(1.0, 1.0, 0.0));
		}
	}

	void Game::DoCollisions()
	{
		for (auto& box : Levels.at(Level).Bricks)
		{
			if (!box.Destroyed)
			{
				Collision collision = CheckCollision(*Ball, box);
				if (collision.hit)
				{
					// destroy block if not solid
					if (!box.IsSolid)
					{
						box.Destroyed = true;
						SpawnPowerUps(box);
						SoundEngine->play2D("res/audio/bleep.mp3", false);
					}
					else
					{
						// if block is solid, enable shake effect
						ShakeTime = 0.05f;
						Effects->Shake = true;
						SoundEngine->play2D("res/audio/solid.wav", false);
					}
					// collision resolution
					Direction dir = collision.direction;
					glm::vec2 diff_vector = collision.difference;
					if (!(Ball->PassThrough && !box.IsSolid))
					{
						if (dir == Direction::LEFT || dir == Direction::RIGHT)
						{
							// horizontal collision
							Ball->Velocity.x = -Ball->Velocity.x;  // reverse horizontal velocity
							// relocate
							float penetration = Ball->Radius - std::abs(diff_vector.x);
							if (dir == Direction::LEFT)
								Ball->Position.x += penetration;  // move ball to right
							else
								Ball->Position.x -= penetration;  // move ball to left
						}
						else
						{
							// vertical collision
							Ball->Velocity.y = -Ball->Velocity.y;  // reverse vertical velocity
							// relocate
							float penetration = Ball->Radius - std::abs(diff_vector.y);
							if (dir == Direction::UP)
								Ball->Position.y -= penetration;  // move ball back up
							else
								Ball->Position.y += penetration;  // move ball back down
						}
					}
				}
			}
		}

		// also check collisions on PowerUps and if so, active them
		for (auto& powerup : PowerUps)
		{
			if (!powerup.Destroyed)
			{
				// first check if powerup passed bottom edge, if so: keep as inactive and destroy
				if (powerup.Position.y >= this->Height)
					powerup.Destroyed = true;
				if (CheckCollision(*Player, powerup))
				{
					// collided with player, now activate powerup
					ActivatePowerUp(powerup);
					powerup.Destroyed = true;
					powerup.Activated = true;
					SoundEngine->play2D("res/audio/powerup.wav", false);
				}
			}
		}

		// check collisions for player pad (unless stuck)
		Collision collision = CheckCollision(*Ball, *Player);
		if (!Ball->Stuck && collision.hit)
		{
			// check where it hit the board, and change velocity based on where it hit the board
			float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
			float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
			float percentage = distance / (Player->Size.x / 2.0f);
			// then move accordingly
			float strength = 2.0f;
			glm::vec2 oldVelocity = Ball->Velocity;
			Ball->Velocity.x = Initial_Ball_Velocity.x * percentage * strength;
			//Ball->Velocity.y = -Ball->Velocity.y;
			// keep speed consistent over both axes (multiply by length of old velocity, so total strength is not changed)
			Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
			// fix sticky paddle
			Ball->Velocity.y = -1.0f * std::abs(Ball->Velocity.y);

			// if stick powerup is activated, also stick ball to paddle once new velocity vectors were calculated
			Ball->Stuck = Ball->Sticky;

			SoundEngine->play2D("res/bleep.wav", false);
		}

	}

	void Game::ResetLevel()
	{
		if (Level == 0) Levels[0].Load("res/levels/one.lvl", Width, Height);
		else if (Level == 1) Levels[1].Load("res/levels/two.lvl", Width, Height);
		else if (Level == 2) Levels[2].Load("res/levels/three.lvl", Width, Height);
		else if (Level == 3) Levels[3].Load("res/levels/four.lvl", Width, Height);
		this->Lives = 3;
	}

	void Game::ResetPlayer()
	{
		// reset player/ball status
		Player->Size = Player_Size;
		Player->Position = glm::vec2(Width / 2.0f - Player_Size.x / 2.0f, Height - Player_Size.y);
		Ball->Reset(Player->Position + glm::vec2(Player_Size.x / 2.0f - Ball_Radius, -(Ball_Radius * 2.0f)),
			Initial_Ball_Velocity);
		// also disable all active powerups
		Effects->Chaos = Effects->Confuse = false;
		Ball->PassThrough = Ball->Sticky = false;
		Player->Color = glm::vec3(1.0f);
		Ball->Color = glm::vec3(1.0f);
	}

	bool Game::ShouldSpawn(unsigned int chance)
	{
		unsigned int random = rand() % chance;
		return random == 0;
	}

	void Game::SpawnPowerUps(GameObject& block)
	{
		if (ShouldSpawn(75))
		{
			// 1 in 75 chance
			PowerUps.push_back(
				PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position, 
					ResourceManager::Textures["powerup_speed"])
			);
		}
		if (ShouldSpawn(75))
		{
			// 1 in 75 chance
			PowerUps.push_back(
				PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position, 
					ResourceManager::Textures["powerup_sticky"])
			);
		}
		if (ShouldSpawn(75))
		{
			// 1 in 75 chance
			PowerUps.push_back(
				PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position, 
					ResourceManager::Textures["powerup_passthrough"])
			);
		}
		if (ShouldSpawn(75))
		{
			// 1 in 75 chance
			PowerUps.push_back(
				PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4f), 0.0f, block.Position, 
					ResourceManager::Textures["powerup_increase"])
			);
		}
		if (ShouldSpawn(15))
		{
			// 1 in 75 chance
			PowerUps.push_back(
				PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.Position, 
					ResourceManager::Textures["powerup_confuse"])
			);
		}
		if (ShouldSpawn(15))
		{
			// 1 in 75 chance
			PowerUps.push_back(
				PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.Position, 
					ResourceManager::Textures["powerup_chaos"])
			);
		}
	}

	void Game::UpdatePowerUps(float dt)
	{
		for (auto& powerup : PowerUps)
		{
			powerup.Position += powerup.Velocity * dt;
			if (powerup.Activated)
			{
				powerup.Duration -= dt;

				if (powerup.Duration <= 0.0f)
				{
					// remove powerup from list (will later be removed)
					powerup.Activated = false;
					if (powerup.Type == "sticky")
					{
						if (!IsOtherPowerUpActive("sticky"))
						{
							// only reset if no other PowerUp of type sticky is active
							Ball->Sticky = false;
							Player->Color = glm::vec3(1.0f);
						}
					}
					else if (powerup.Type == "pass-through")
					{
						if (!IsOtherPowerUpActive("pass-through"))
						{
							Ball->PassThrough = false;
							Ball->Color = glm::vec3(1.0f);
						}
					}
					else if (powerup.Type == "confuse")
					{
						if (!IsOtherPowerUpActive("confuse"))
						{
							Effects->Confuse = false;
						}
					}
					else if (powerup.Type == "chaos")
					{
						if (!IsOtherPowerUpActive("chaos"))
						{
							Effects->Chaos = false;
						}
					}
				}
			}
		}

		PowerUps.erase(std::remove_if(PowerUps.begin(), PowerUps.end(),
			[](const PowerUp& powerup) {return powerup.Destroyed && !powerup.Activated; }
		), PowerUps.end());
	}

	void Game::ActivatePowerUp(PowerUp& powerup)
	{
		if (powerup.Type == "speed") Ball->Velocity *= 1.2;
		else if (powerup.Type == "sticky")
		{
			Ball->Sticky = true;
			Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
		}
		else if (powerup.Type == "pass-through")
		{
			Ball->PassThrough = true;
			Ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
		}
		else if (powerup.Type == "pad-size-increase")
		{
			Player->Size.x += 50;
		}
		else if (powerup.Type == "confuse")
		{
			// only activate if chaos wasn't already active
			if (!Effects->Chaos) Effects->Confuse = true;
		}
		else if (powerup.Type == "chaos")
		{
			if (!Effects->Confuse) Effects->Chaos = true;
		}
	}

	bool Game::IsOtherPowerUpActive(const std::string& type)
	{
		for (const auto& powerup : PowerUps)
		{
			if (powerup.Activated)
			{
				if (powerup.Type == type) return true;
			}
		}
		return false;
	}

	bool Game::CheckCollision(GameObject& one, GameObject& two)
	{
		// AABB - AABB collision
		// collision x-axis
		bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
			two.Position.x + two.Size.x >= one.Position.x;
		// collision y-axis
		bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
			two.Position.y + two.Position.y >= one.Position.y;
		// collision only if on both axes
		return collisionX && collisionY;
	}

	Collision Game::CheckCollision(BallObject& one, GameObject& two)
	{
		// AABB - Circle collision
		// get center point circle first
		glm::vec2 center(one.Position + one.Radius);
		// calculate AABB info (center, half-extents)
		glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
		glm::vec2 aabb_center(
			two.Position.x + aabb_half_extents.x,
			two.Position.y + aabb_half_extents.y
		);
		// get difference vector between both centers
		glm::vec2 difference = center - aabb_center;
		glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
		// add clamped value to AABB_center and we get the value of box closest to circle
		glm::vec2 closest = aabb_center + clamped;
		// retrieve vector between center circle and closest point AABB and check if length <= radius
		difference = closest - center;
		
		if (glm::length(difference) < one.Radius)
			return { true, VectorDirection(difference), difference };
		else
			return { false, Direction::UP, glm::vec2(0.0f, 0.0f) };
	}

	breakout::Direction Game::VectorDirection(glm::vec2 target)
	{
		glm::vec2 compass[4] = {
			glm::vec2(0.0f, 1.0f),  // up
			glm::vec2(1.0f, 0.0f),  // right
			glm::vec2(0.0f, -1.0f), // down
			glm::vec2(-1.0f, 0.0f)  // left
		};

		float max = 0.0f;
		unsigned int best_match = -1;
		for (unsigned int i = 0; i < 4; ++i)
		{
			float dot_product = glm::dot(glm::normalize(target), compass[i]);
			if (dot_product > max)
			{
				max = dot_product;
				best_match = i;
			}
		}
		return static_cast<Direction>(best_match);
	}

	std::unique_ptr<breakout::SpriteRenderer> Game::Renderer;

	std::unique_ptr<breakout::GameObject> Game::Player;

	std::unique_ptr<breakout::BallObject> Game::Ball;

	std::unique_ptr<breakout::ParticleGenerator> Game::Particles;

	std::unique_ptr<breakout::PostProcessor> Game::Effects;

	std::unique_ptr<breakout::TextRenderer> Game::Text;

	glm::vec2 Game::Player_Size = glm::vec2(100.0f, 20.0f);

	float Game::Player_Velocity = 500.0f;

	glm::vec2 Game::Initial_Ball_Velocity = glm::vec2(100.0f, -350.0f);

	float Game::Ball_Radius = 12.5f;

	float Game::ShakeTime = 0.0f;

}