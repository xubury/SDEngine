#include "Renderer/ParticleSystem.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Utility/Random.hpp"
#include "Utility/Math.hpp"

namespace SD {

ParticleSystem::ParticleSystem(int poolSize)
    : RenderSystem("Particle"), m_poolIndex(poolSize - 1)
{
    m_particles.resize(poolSize);
}

void ParticleSystem::OnPush() {}

void ParticleSystem::OnPop() {}

void ParticleSystem::OnRender()
{
    int index = 0;
    Renderer::BeginRenderSubpass(RenderSubpassInfo{&index, 1});
    Renderer2D::Begin(GetCamera());
    for (const auto &particle : m_particles) {
        if (!particle.active) continue;

        float life = particle.lifeRemaining / particle.lifeTime;
        Vector4f color =
            Math::Lerp(particle.colorEnd, particle.colorBegin, life);

        float size = Math::Lerp(particle.sizeEnd, particle.sizeBegin, life);

        Matrix4f transform =
            glm::translate(Matrix4f(1.0f), particle.position) *
            glm::rotate(Matrix4f(1.0f), particle.rotation,
                        Vector3f(0.0f, 0.0f, 1.0f)) *
            glm::scale(Matrix4f(1.0f), Vector3f(size, size, 1.0f));
        Renderer2D::DrawQuad(transform, color);
    }
    Renderer::EndRenderSubpass();
}

void ParticleSystem::OnTick(float dt)
{
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

void ParticleSystem::Emit(const ParticleProp &particleProps)
{
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
