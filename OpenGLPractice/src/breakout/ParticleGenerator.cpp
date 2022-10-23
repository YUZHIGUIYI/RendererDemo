#include "ParticleGenerator.h"
#include <glad/glad.h>

namespace breakout
{
	unsigned int ParticleGenerator::lastUsedParticle = 0;

	ParticleGenerator::ParticleGenerator(Renderer::Shader* shader, Renderer::Texture2D* texture, unsigned int amount)
		: shader(shader), texture(texture), amount(amount)
	{
		init();
	}

	void ParticleGenerator::Update(float dt, GameObject& object, unsigned int newParticles, glm::vec2 offset /*= glm::vec2(0.0f, 0.0f)*/)
	{
		// add new particles
		for (unsigned int i = 0; i < newParticles; ++i)
		{
			int unusedParticle = firstUnusedParticle();
			respawnParticle(particles[unusedParticle], object, offset);
		}
		// update all particles
		for (unsigned int i = 0; i < amount; ++i)
		{
			Particle& p = particles[i];
			p.Life -= dt;  // reduce life
			if (p.Life > 0.0f)
			{
				// particle is alive, then update
				p.Position -= p.Velocity * dt;
				p.Color.a -= dt * 2.5f;
			}
		}
	}

	void ParticleGenerator::Draw()
	{
		// use additive blending to give it a 'glow' offset
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		shader->Bind();
		for (auto& particle : particles)
		{
			if (particle.Life > 0.0f)
			{
				shader->SetUniformVec2f("offset", particle.Position);
				shader->SetUniformVec4f("color", particle.Color);
				texture->Bind();
				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glBindVertexArray(0);
			}
		}
		// don't forget to reset to default blending mode
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void ParticleGenerator::init()
	{
		// set up mesh and attribute properties
		unsigned int VBO;
		float particle_quad[] = {
			0.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,

			0.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 0.0f
		};
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(this->VAO);
		// fill mesh buffer
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
		// set mesh attributes
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glBindVertexArray(0);

		// create this->amount default particle instances
		particles.reserve(amount);
		for (unsigned int i = 0; i < amount; ++i)
		{
			particles.push_back(Particle());
		}
	}

	unsigned int ParticleGenerator::firstUnusedParticle()
	{
		// first search from last used particle, this will usually return almost instantly
		for (unsigned int i = lastUsedParticle; i < this->amount; ++i) {
			if (this->particles[i].Life <= 0.0f) {
				lastUsedParticle = i;
				return i;
			}
		}
		// otherwise, do a linear search
		for (unsigned int i = 0; i < lastUsedParticle; ++i) {
			if (this->particles[i].Life <= 0.0f) {
				lastUsedParticle = i;
				return i;
			}
		}
		// all particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
		lastUsedParticle = 0;
		return 0;
	}

	void ParticleGenerator::respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset /*= glm::vec2(0.0f, 0.0f)*/)
	{
		float random = ((rand() % 100) - 50) / 10.0f;
		float rColor = 0.5f + ((rand() % 100) / 100.0f);
		particle.Position = object.Position + random + offset;
		particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
		particle.Life = 1.0f;
		particle.Velocity = object.Velocity * 0.1f;
	}

}