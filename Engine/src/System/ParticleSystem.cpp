#include "System/ParticleSystem.hpp"
#include "Utility/Random.hpp"
#include "Utility/Math.hpp"

#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

namespace SD {

ParticleSystem::ParticleSystem(int poolSize)
    : System("Particle"), m_poolIndex(poolSize - 1) {
    m_particles.resize(poolSize);
}

void ParticleSystem::OnPush() {}

void ParticleSystem::OnPop() {}

void ParticleSystem::OnRender() {
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
        renderer->DrawQuad(transform, color);
    }
}

void ParticleSystem::OnTick(float dt) {
    for (auto &particle : m_particles) {
        if (!particle.active) continue;

        if (particle.lifeRemaining <= 0.0f) {
            particle.active = false;
            continue;
        }

        particle.lifeRemaining -= dt;
        particle.position += particle.velocity * dt;
        particle.rotation += dt * 180.0f / Math::PI;
    }
}

void ParticleSystem::Emit(const ParticleProp &particleProps) {
    Particle &particle = m_particles[m_poolIndex];

    particle.active = true;

    particle.position = particleProps.position;
    particle.rotation = Random::Rnd(0.f, 1.0f) * Math::PI * 2.0f;
    particle.velocity = particleProps.velocity;
    particle.velocity.x +=
        particleProps.velocityVariation.x * Random::Rnd(-1.0f, 1.0f);
    particle.velocity.y +=
        particleProps.velocityVariation.y * Random::Rnd(-1.0f, 1.0f);

    particle.colorBegin = particleProps.colorBegin;
    particle.colorEnd = particleProps.colorEnd;

    particle.lifeTime = particleProps.lifeTime;
    particle.lifeRemaining = particleProps.lifeTime;

    particle.sizeBegin = particleProps.sizeBegin;
    particle.sizeEnd = particleProps.sizeEnd;
    particle.sizeBegin +=
        particleProps.sizeVariation * Random::Rnd(-1.0f, 1.0f);
    particle.sizeEnd += particleProps.sizeVariation * Random::Rnd(-1.0f, 1.0f);

    m_poolIndex = (m_poolIndex - 1) % m_particles.size();
}

}  // namespace SD
