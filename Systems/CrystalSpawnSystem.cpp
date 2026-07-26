#include "Systems/CrystalSpawnSystem.h"

#include <random>

#include "Components/ZigZagBallComponent.h"
#include "Context/ZigZagContext.h"
#include "Ecs/Components/MaterialComponent.h"
#include "Ecs/Components/MeshComponent.h"
#include "Ecs/Components/ShaderComponent.h"
#include "Ecs/Components/TransformComponent.h"
#include "Ecs/Core/EcsWorld.h"
#include "Engine/Engine.h"
#include "Resource/MeshFactory.h"
#include "Systems/ParticleBurstSystem.h"
#include "Systems/ScoreSystem.h"
#include "Systems/TileSpawnSystem.h"

CrystalSpawnSystem::CrystalSpawnSystem(TileSpawnSystem& tileSpawnSystem, ParticleBurstSystem& particleBurstSystem, ScoreSystem& scoreSystem)
    : tileSpawnSystem(tileSpawnSystem), particleBurstSystem(particleBurstSystem), scoreSystem(scoreSystem)
{
}

void CrystalSpawnSystem::Reset(ZigZagContext& context)
{
    activeCrystals.clear();
    freeCrystalEntities.clear();
    spinAngle = 0.0f;
}

void CrystalSpawnSystem::Init(ZigZagContext& context)
{
    Reset(context);

    if (!crystalVao)
        crystalVao = MeshFactory::CreateGem(context.engine->GetRenderEngine(), crystalIndexCount, 6, context.crystalRadius, context.crystalRadius * 0.9f, context.crystalRadius * 1.8f);

    constexpr size_t poolReserve = 24;
    while (freeCrystalEntities.size() < poolReserve)
        freeCrystalEntities.push_back(CreateCrystalEntity(context));
}

Entity CrystalSpawnSystem::AcquireCrystalEntity(ZigZagContext& context)
{
    if (!freeCrystalEntities.empty())
    {
        const Entity entity = freeCrystalEntities.back();
        freeCrystalEntities.pop_back();
        return entity;
    }

    return CreateCrystalEntity(context);
}

Entity CrystalSpawnSystem::CreateCrystalEntity(ZigZagContext& context)
{
    auto& world = context.engine->GetWorld();

    const Entity entity = world.CreateEntity();

    auto& transform = world.AddComponent<TransformComponent>(entity);
    transform.scale = {0.0f, 0.0f, 0.0f};

    auto& mesh = world.AddComponent<MeshComponent>(entity);
    mesh.vao = crystalVao;
    mesh.indexCount = crystalIndexCount;

    auto& material = world.AddComponent<MaterialComponent>(entity);
    material.diffuseColor = context.crystalColor;
    material.emissiveColor = {context.crystalColor.x, context.crystalColor.y, context.crystalColor.z};
    material.emissiveIntensity = 1.3f;

    auto& shaderComp = world.AddComponent<ShaderComponent>(entity);
    shaderComp.shader = context.litShader;
    shaderComp.shaderType = ShaderRenderType::Lit;

    context.allEntities.push_back(entity);
    return entity;
}

void CrystalSpawnSystem::NotifyTileCreated(ZigZagContext& context, uint64_t tileId, const Vector3& center)
{
    std::uniform_real_distribution<float> chanceDist(0.0f, 1.0f);
    if (chanceDist(context.rng) >= context.crystalSpawnChance)
        return;

    const Entity entity = AcquireCrystalEntity(context);

    auto& world = context.engine->GetWorld();
    auto& transform = world.GetComponent<TransformComponent>(entity);
    transform.position = {center.x, context.platformTopY + context.crystalFloatHeight, center.z};
    transform.scale = {1.0f, 1.0f, 1.0f};
    transform.rotation.SetIdentity();

    activeCrystals.push_back({tileId, entity, transform.position});
}

void CrystalSpawnSystem::NotifyTileRetired(ZigZagContext& context, uint64_t tileId)
{
    for (size_t i = 0; i < activeCrystals.size(); i++)
    {
        if (activeCrystals[i].tileId != tileId)
            continue;

        auto& world = context.engine->GetWorld();
        auto& transform = world.GetComponent<TransformComponent>(activeCrystals[i].entity);
        transform.scale = {0.0f, 0.0f, 0.0f};
        freeCrystalEntities.push_back(activeCrystals[i].entity);

        activeCrystals[i] = activeCrystals.back();
        activeCrystals.pop_back();
        return;
    }
}

void CrystalSpawnSystem::CollectCrystal(ZigZagContext& context, size_t slotIndex)
{
    auto& world = context.engine->GetWorld();
    const CrystalSlot slot = activeCrystals[slotIndex];

    particleBurstSystem.SpawnBurst(context, slot.position, context.crystalColor);

    auto& transform = world.GetComponent<TransformComponent>(slot.entity);
    transform.scale = {0.0f, 0.0f, 0.0f};
    freeCrystalEntities.push_back(slot.entity);

    activeCrystals[slotIndex] = activeCrystals.back();
    activeCrystals.pop_back();

    scoreSystem.AddCrystalScore(context);
}

void CrystalSpawnSystem::Update(ZigZagContext& context, float deltaTime)
{
    if (activeCrystals.empty() || context.ballEntity == 0)
        return;

    auto& world = context.engine->GetWorld();

    spinAngle += spinSpeed * deltaTime;
    if (spinAngle > 6.2831853f)
        spinAngle -= 6.2831853f;

    const auto& ball = world.GetComponent<ZigZagBallComponent>(context.ballEntity);
    const auto& ballTransform = world.GetComponent<TransformComponent>(context.ballEntity);
    const float collectDist = context.ballRadius + context.crystalRadius;
    const float collectDistSq = collectDist * collectDist;

    for (size_t i = 0; i < activeCrystals.size();)
    {
        auto& transform = world.GetComponent<TransformComponent>(activeCrystals[i].entity);
        transform.rotation.SetRotationY(spinAngle);

        if (ball.alive)
        {
            const float dx = transform.position.x - ballTransform.position.x;
            const float dz = transform.position.z - ballTransform.position.z;
            if (dx * dx + dz * dz <= collectDistSq)
            {
                CollectCrystal(context, i);
                continue;
            }
        }

        i++;
    }
}
