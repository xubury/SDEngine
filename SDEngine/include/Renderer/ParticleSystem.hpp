#ifndef SD_PARTICLE_SYSTEM_HPP
#define SD_PARTICLE_SYSTEM_HPP

#include "Renderer/Export.hpp"
#include "Renderer/RenderSystem.hpp"
#include "Graphics/Camera.hpp"

#include <vector>

namespace SD {

struct SD_RENDERER_API ParticleProp {
    Vector3f position;
    Vector3f velocity;
    Vector3f velocityVariation;
    Vector4f colorBegin;
    Vector4f colorEnd;
    float sizeBegin;
    float sizeEnd;
    float sizeVariation;
    float lifeTime;
    ParticleProp()
        : position(0.f),
          velocity(0.f),
          velocityVariation(0.f),
          colorBegin(1.0f),
          colorEnd(1.0f),
          sizeBegin(1.0f),
          sizeEnd(1.0f),
          sizeVariation(0.f),
          lifeTime(1.0f)
    {
    }
};

class SD_RENDERER_API ParticleSystem : public RenderSystem {
   public:
    ParticleSystem(int poolSize = 1000);

    void Emit(const ParticleProp &particle);

    void OnPush() override;

    void OnPop() override;

    void OnRender() override;

    void OnTick(float dt) override;

   private:
    struct Particle {
        Vector3f position;
        Vector3f velocity;
        Vector4f colorBegin;
        Vector4f colorEnd;

        float rotation;

        float sizeBegin;
        float sizeEnd;

        float lifeTime = 1.0f;
        float lifeRemaining = 0.f;

        bool active;

        Particle() : lifeTime(1.0f), lifeRemaining(0.f), active(false) {}
    };
    size_t m_poolIndex;
    std::vector<Particle> m_particles;
};

}  // namespace SD

#endif /* SD_PARTICLE_SYSTEM_HPP */
