#pragma once

#include "Ecs/Core/Entity.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include <vector>

struct ZigZagContext;

class ParticleBurstSystem
{
public:
    void Init(ZigZagContext& context);
    void Update(ZigZagContext& context, float deltaTime);
    void Reset(ZigZagContext& context);

    void SpawnBurst(ZigZagContext& context, const Vector3& position, const Vector4& color);

private:
    struct Burst
    {
        Entity entity = 0;
        float timeRemaining = 0.0f;
    };

    std::vector<Burst> bursts;
};
