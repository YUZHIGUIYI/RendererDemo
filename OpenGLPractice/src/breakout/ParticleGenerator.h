#pragma once

#include "Shader.h"
#include "Texture2D.h"
#include "GameObject.h"

namespace breakout
{
	// Represents a single particle and its state
	struct Particle
	{
		glm::vec2 Position, Velocity;
		glm::vec4 Color;
		float Life;

		Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) {}
	};

	// ParticleGenerator acts as a container for rendering a large number of 
	// particles by repeatedly spawning and updating particles and killing 
	// them after a given amount of time.
	class ParticleGenerator
	{
	public:
		ParticleGenerator(Renderer::Shader* shader, Renderer::Texture2D* texture, unsigned int amount);

		// update all particles
		void Update(float dt, GameObject& object, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));

		// render all particles
		void Draw();

	public:
		static unsigned int lastUsedParticle;

	private:
		// state 
		std::vector<Particle> particles;
		unsigned int amount;
		
		// render state
		Renderer::Shader* shader;
		Renderer::Texture2D* texture;
		unsigned int VAO;

		// initializes buffer and vertex attributes
		void init();
		// returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
		unsigned int firstUnusedParticle();
		// respawns particle
		void respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
	};
}