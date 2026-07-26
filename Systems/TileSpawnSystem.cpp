#include "Systems/TileSpawnSystem.h"

#include <cmath>
#include <random>

#include "Context/ZigZagContext.h"
#include "Ecs/Components/MaterialComponent.h"
#include "Ecs/Components/MeshComponent.h"
#include "Ecs/Components/ShaderComponent.h"
#include "Ecs/Components/TransformComponent.h"
#include "Ecs/Core/EcsWorld.h"
#include "Engine/Engine.h"
#include "Resource/MeshFactory.h"

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

    if (!tileVao)
        tileVao = MeshFactory::CreateCube(context.engine->GetRenderEngine(), tileIndexCount, 1.0f);

    constexpr size_t initialLookAhead = 20;
    constexpr size_t poolReserve = 48;

    while (freeTileEntities.size() < poolReserve)
        freeTileEntities.push_back(CreateTileEntity(context));

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

    return CreateTileEntity(context);
}

Entity TileSpawnSystem::CreateTileEntity(ZigZagContext& context)
{
    auto& world = context.engine->GetWorld();

    const Entity entity = world.CreateEntity();

    auto& transform = world.AddComponent<TransformComponent>(entity);
    transform.scale = {0.0f, 0.0f, 0.0f};

    auto& mesh = world.AddComponent<MeshComponent>(entity);
    mesh.vao = tileVao;
    mesh.indexCount = tileIndexCount;
    mesh.visible = false;

    auto& material = world.AddComponent<MaterialComponent>(entity);
    material.diffuseColor = context.zigZagColor;

    auto& shaderComp = world.AddComponent<ShaderComponent>(entity);
    shaderComp.shader = context.litShader;
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
        world.GetComponent<MeshComponent>(entity).visible = true;

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
            world.GetComponent<MeshComponent>(falling.entity).visible = false;
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
