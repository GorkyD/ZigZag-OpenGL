#include "Systems/TileSpawnSystem.h"

#include <cmath>
#include <random>

#include "Context/ZigZagContext.h"
#include "Ecs/Components/MaterialComponent.h"
#include "Ecs/Components/ShaderComponent.h"
#include "Ecs/Components/TransformComponent.h"
#include "Ecs/Core/EcsWorld.h"
#include "Engine/Engine.h"
#include "Resource/EntityFactory.h"

void TileSpawnSystem::Reset(ZigZagContext& context)
{
    pathTiles.clear();
    fallingTiles.clear();
    freeTileEntities.clear();
    nextTileId = 1;
    movingRight = false;
    pathCursor = {0.0f, 0.0f, 0.0f};
}

void TileSpawnSystem::Init(ZigZagContext& context)
{
    Reset(context);
    pathCursor = {0.0f, 0.0f, context.platformHalfSize - context.tileSize * 0.5f};

    constexpr size_t initialLookAhead = 20;
    while (pathTiles.size() < initialLookAhead)
        GenerateSegment(context);
}

void TileSpawnSystem::SetOnTileCreated(std::function<void(uint64_t, const Vector3&)> callback)
{
    onTileCreated = std::move(callback);
}

void TileSpawnSystem::SetOnTileRetired(std::function<void(uint64_t)> callback)
{
    onTileRetired = std::move(callback);
}

Entity TileSpawnSystem::AcquireTileEntity(ZigZagContext& context)
{
    if (!freeTileEntities.empty())
    {
        const Entity entity = freeTileEntities.back();
        freeTileEntities.pop_back();
        return entity;
    }

    auto& world = context.engine->GetWorld();
    auto* renderEngine = context.engine->GetRenderEngine();

    const auto entity = EntityFactory::CreateModelEntity(world, renderEngine, "Assets/Models/cube.obj", context.litShader);

    auto& material = world.GetComponent<MaterialComponent>(entity);
    material.diffuseTexture = nullptr;
    material.diffuseColor = context.zigZagColor;

    auto& shaderComp = world.GetComponent<ShaderComponent>(entity);
    shaderComp.shaderType = ShaderRenderType::Lit;

    context.allEntities.push_back(entity);
    return entity;
}

void TileSpawnSystem::GenerateSegment(ZigZagContext& context)
{
    auto& world = context.engine->GetWorld();

    std::uniform_int_distribution<int> lengthDist(1, 3);
    const int tilesInSegment = lengthDist(context.rng);

    const Vector3 dir = movingRight ? context.rightDir : context.forwardDir;

    for (int tile = 0; tile < tilesInSegment; tile++)
    {
        pathCursor += dir * context.tileSize;

        const Entity entity = AcquireTileEntity(context);

        auto& transform = world.GetComponent<TransformComponent>(entity);
        transform.position = {pathCursor.x, context.platformTopY - context.tileHeight * 0.5f, pathCursor.z};
        transform.scale = {context.tileSize, context.tileHeight, context.tileSize};

        const uint64_t tileId = nextTileId++;
        pathTiles.push_back({tileId, entity, pathCursor});

        if (onTileCreated)
            onTileCreated(tileId, pathCursor);
    }

    movingRight = !movingRight;
}

void TileSpawnSystem::RetireFrontTile(ZigZagContext& context)
{
    if (pathTiles.empty())
        return;

    const PathTile tile = pathTiles.front();
    pathTiles.pop_front();

    if (onTileRetired)
        onTileRetired(tile.id);

    fallingTiles.push_back({tile.entity, 0.0f});
}

void TileSpawnSystem::MaintainPath(ZigZagContext& context, size_t currentTileIndex)
{
    constexpr size_t trailingKeep = 2;
    while (currentTileIndex > trailingKeep && !pathTiles.empty())
    {
        RetireFrontTile(context);
        currentTileIndex--;
    }

    constexpr size_t lookAheadTiles = 20;
    while (pathTiles.size() - currentTileIndex < lookAheadTiles)
        GenerateSegment(context);
}

bool TileSpawnSystem::IsOverFootprint(const ZigZagContext& context, const Vector3& position, size_t* outTileIndex) const
{
    if (std::abs(position.x) <= context.platformHalfSize && std::abs(position.z) <= context.platformHalfSize)
        return true;

    const float half = context.tileSize * 0.5f + context.ballRadius * 0.8f;
    for (size_t i = 0; i < pathTiles.size(); i++)
    {
        const Vector3& tile = pathTiles[i].center;
        if (std::abs(position.x - tile.x) <= half && std::abs(position.z - tile.z) <= half)
        {
            if (outTileIndex)
                *outTileIndex = i;
            return true;
        }
    }

    return false;
}

void TileSpawnSystem::Update(ZigZagContext& context, float deltaTime)
{
    if (fallingTiles.empty())
        return;

    auto& world = context.engine->GetWorld();

    for (size_t i = 0; i < fallingTiles.size();)
    {
        FallingTile& falling = fallingTiles[i];
        falling.velocity += 20.0f * deltaTime;

        auto& transform = world.GetComponent<TransformComponent>(falling.entity);
        transform.position.y -= falling.velocity * deltaTime;

        if (transform.position.y < context.platformTopY - 8.0f)
        {
            transform.scale = {0.0f, 0.0f, 0.0f};
            freeTileEntities.push_back(falling.entity);

            fallingTiles[i] = fallingTiles.back();
            fallingTiles.pop_back();
        }
        else
        {
            i++;
        }
    }
}
