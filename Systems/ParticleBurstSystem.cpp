#include "Systems/ParticleBurstSystem.h"

#include <cmath>
#include <random>

#include "Context/ZigZagContext.h"
#include "Ecs/Components/ParticleEmitterComponent.h"
#include "Ecs/Components/TransformComponent.h"
#include "Ecs/Core/EcsWorld.h"
#include "Engine/Engine.h"

void ParticleBurstSystem::Reset(ZigZagContext& context)
{
    bursts.clear();
}

void ParticleBurstSystem::Init(ZigZagContext& context)
{
    Reset(context);
}

void ParticleBurstSystem::SpawnBurst(ZigZagContext& context, const Vector3& position, const Vector4& color)
{
    auto& world = context.engine->GetWorld();

    const Entity entity = world.CreateEntity();

    world.AddComponent<TransformComponent>(entity);

    auto& emitter = world.AddComponent<ParticleEmitterComponent>(entity);
    emitter.color = color;
    emitter.additive = true;
    emitter.spawnRate = 0.0f;
    emitter.startSize = 0.28f;
    emitter.endSize = 0.02f;
    emitter.lifetimeMin = 0.45f;
    emitter.lifetimeMax = 0.85f;
    emitter.gravity = {0.0f, -0.6f, 0.0f};

    constexpr int burstCount = 32;
    emitter.particles.reserve(burstCount);

    std::uniform_real_distribution<float> angleDist(0.0f, 6.2831853f);
    std::uniform_real_distribution<float> speedDist(2.5f, 6.5f);
    std::uniform_real_distribution<float> upDist(1.0f, 4.5f);
    std::uniform_real_distribution<float> lifetimeDist(0.45f, 0.85f);

    for (int i = 0; i < burstCount; i++)
    {
        const float angle = angleDist(context.rng);
        const float speed = speedDist(context.rng);

        Particle particle;
        particle.position = position;
        particle.velocity = {std::cos(angle) * speed, upDist(context.rng), std::sin(angle) * speed};
        particle.lifetime = lifetimeDist(context.rng);
        particle.age = 0.0f;
        emitter.particles.push_back(particle);
    }

    context.allEntities.push_back(entity);
    bursts.push_back({entity, 1.0f});
}

void ParticleBurstSystem::Update(ZigZagContext& context, float deltaTime)
{
    if (bursts.empty())
        return;

    auto& world = context.engine->GetWorld();

    for (size_t i = 0; i < bursts.size();)
    {
        bursts[i].timeRemaining -= deltaTime;
        if (bursts[i].timeRemaining <= 0.0f)
        {
            world.DestroyEntity(bursts[i].entity);
            bursts[i] = bursts.back();
            bursts.pop_back();
        }
        else
        {
            i++;
        }
    }
}
