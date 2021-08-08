#include "Renderer/ParticleSystem.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Utils/Random.hpp"

#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

namespace sd {

ParticleSystem::ParticleSystem(int poolSize) : m_poolIndex(poolSize - 1) {
    m_particles.resize(poolSize);
}

void ParticleSystem::onRender() {
    for (const auto &particle : m_particles) {
        if (!particle.active) continue;

        float life = particle.lifeRemaining / particle.lifeTime;
        glm::vec4 color =
            glm::lerp(particle.colorEnd, particle.colorBegin, life);

        float size = glm::lerp(particle.sizeEnd, particle.sizeBegin, life);

        glm::mat4 transform =
            glm::translate(glm::mat4(1.0f), particle.position) *
            glm::rotate(glm::mat4(1.0f), particle.rotation,
                        glm::vec3(0.0f, 0.0f, 1.0f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(size, size, 1.0f));
        Renderer2D::drawQuad(transform, color);
    }
}

void ParticleSystem::onTick(float dt) {
    for (auto &particle : m_particles) {
        if (!particle.active) continue;

        if (particle.lifeRemaining <= 0.0f) {
            particle.active = false;
            continue;
        }

        particle.lifeRemaining -= dt;
        particle.position += particle.velocity * dt;
        // particle.rotation += dt * 180.0f / M_PI;
    }
}

void ParticleSystem::emit(const ParticleProp &particleProps) {
    Particle &particle = m_particles[m_poolIndex];

    particle.active = true;

    particle.position = particleProps.position;
    particle.rotation = Random::rnd(0.f, 1.0f) * M_PI * 2.0f;
    particle.velocity = particleProps.velocity;
    particle.velocity.x +=
        particleProps.velocityVariation.x * Random::rnd(-1.0f, 1.0f);
    particle.velocity.y +=
        particleProps.velocityVariation.y * Random::rnd(-1.0f, 1.0f);

    particle.colorBegin = particleProps.colorBegin;
    particle.colorEnd = particleProps.colorEnd;

    particle.lifeTime = particleProps.lifeTime;
    particle.lifeRemaining = particleProps.lifeTime;

    particle.sizeBegin = particleProps.sizeBegin;
    particle.sizeEnd = particleProps.sizeEnd;
    particle.sizeBegin +=
        particleProps.sizeVariation * Random::rnd(-1.0f, 1.0f);
    particle.sizeEnd += particleProps.sizeVariation * Random::rnd(-1.0f, 1.0f);

    m_poolIndex = (m_poolIndex - 1) % m_particles.size();
}

}  // namespace sd
