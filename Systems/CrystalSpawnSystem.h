#pragma once

#include "Ecs/Core/Entity.h"
#include "Extension/Extension.h"
#include "Math/Vector3.h"
#include <cstddef>
#include <cstdint>
#include <vector>

struct ZigZagContext;
class TileSpawnSystem;
class ParticleBurstSystem;
class ScoreSystem;

class CrystalSpawnSystem
{
public:
    CrystalSpawnSystem(TileSpawnSystem& tileSpawnSystem, ParticleBurstSystem& particleBurstSystem, ScoreSystem& scoreSystem);

    void Init(ZigZagContext& context);
    void Update(ZigZagContext& context, float deltaTime);
    void Reset(ZigZagContext& context);

    void NotifyTileCreated(ZigZagContext& context, uint64_t tileId, const Vector3& center);
    void NotifyTileRetired(ZigZagContext& context, uint64_t tileId);

private:
    struct CrystalSlot
    {
        uint64_t tileId = 0;
        Entity entity = 0;
        Vector3 position;
    };

    Entity AcquireCrystalEntity(ZigZagContext& context);
    Entity CreateCrystalEntity(ZigZagContext& context);
    void CollectCrystal(ZigZagContext& context, size_t slotIndex);

    TileSpawnSystem& tileSpawnSystem;
    ParticleBurstSystem& particleBurstSystem;
    ScoreSystem& scoreSystem;

    std::vector<CrystalSlot> activeCrystals;
    std::vector<Entity> freeCrystalEntities;

    VertexArrayObjectPtr crystalVao;
    unsigned int crystalIndexCount = 0;

    float spinAngle = 0.0f;
    float spinSpeed = 3.0f;
};
