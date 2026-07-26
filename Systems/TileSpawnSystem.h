#pragma once

#include "Ecs/Core/Entity.h"
#include "Extension/Extension.h"
#include "Math/Vector3.h"
#include <cstddef>
#include <cstdint>
#include <deque>
#include <functional>
#include <vector>

struct ZigZagContext;

class TileSpawnSystem
{
public:
    void Init(ZigZagContext& context);
    void Update(ZigZagContext& context, float deltaTime);
    void Reset(ZigZagContext& context);

    void MaintainPath(ZigZagContext& context, size_t currentTileIndex);
    bool IsOverFootprint(const ZigZagContext& context, const Vector3& position, size_t* outTileIndex = nullptr) const;

    void SetOnTileCreated(std::function<void(uint64_t tileId, const Vector3& center)> callback);
    void SetOnTileRetired(std::function<void(uint64_t tileId)> callback);

private:
    struct PathTile
    {
        uint64_t id = 0;
        Entity entity = 0;
        Vector3 center;
    };

    struct FallingTile
    {
        Entity entity = 0;
        float velocity = 0.0f;
    };

    void GenerateSegment(ZigZagContext& context);
    void RetireFrontTile(ZigZagContext& context);
    Entity AcquireTileEntity(ZigZagContext& context);
    Entity CreateTileEntity(ZigZagContext& context);

    std::deque<PathTile> pathTiles;
    std::vector<FallingTile> fallingTiles;
    std::vector<Entity> freeTileEntities;

    std::function<void(uint64_t, const Vector3&)> onTileCreated;
    std::function<void(uint64_t)> onTileRetired;

    VertexArrayObjectPtr tileVao;
    unsigned int tileIndexCount = 0;

    Vector3 pathCursor = {0.0f, 0.0f, 0.0f};
    uint64_t nextTileId = 1;
    bool movingRight = false;
};
