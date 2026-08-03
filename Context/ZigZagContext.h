#pragma once

#include "Ecs/Core/Entity.h"
#include "Extension/Extension.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include <random>
#include <vector>

class Engine;

struct ZigZagContext
{
    Engine* engine = nullptr;

    ShaderProgramPtr litShader;
    ShaderProgramPtr unlitShader;

    std::mt19937 rng{std::random_device{}()};
    std::vector<Entity> allEntities;

    Entity ballEntity = 0;
    Entity cameraEntity = 0;

    float score = 0.0f;
    int bestScore = 0;

    bool restartRequested = false;

    const Vector4 zigZagColor = {0.15f, 0.45f, 0.95f, 1.0f};
    const Vector4 crystalColor = {1.0f, 0.25f, 0.75f, 1.0f};
    const Vector3 forwardDir = {0.0f, 0.0f, 1.0f};
    const Vector3 rightDir = {1.0f, 0.0f, 0.0f};
    const Vector3 cameraOffset = {-8.0f, 9.0f, -7.0f};

    float platformHalfSize = 3.0f;
    float platformTopY = 0.0f;
    float tileSize = 2.0f;
    float tileHeight = 0.6f;
    float ballRadius = 0.4f;
    float speedGrowthRate = 0.15f;
    float maxBallSpeed = 12.0f;

    float crystalRadius = 0.28f;
    float crystalFloatHeight = 0.55f;
    float crystalSpawnChance = 0.35f;
    float crystalScoreValue = 50.0f;
    float distanceScoreRate = 2.0f;
};
