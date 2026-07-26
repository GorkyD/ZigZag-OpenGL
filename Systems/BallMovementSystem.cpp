#include "Systems/BallMovementSystem.h"

#include <algorithm>
#include <climits>
#include <cmath>

#include "Components/ZigZagBallComponent.h"
#include "Context/ZigZagContext.h"
#include "Ecs/Components/MaterialComponent.h"
#include "Ecs/Components/MeshComponent.h"
#include "Ecs/Components/ShaderComponent.h"
#include "Ecs/Components/TransformComponent.h"
#include "Ecs/Core/EcsWorld.h"
#include "Engine/Engine.h"
#include "Input/InputSystem.h"
#include "Resource/MeshFactory.h"
#include "Systems/ScoreSystem.h"
#include "Systems/TileSpawnSystem.h"

BallMovementSystem::BallMovementSystem(TileSpawnSystem& tileSpawnSystem, ScoreSystem& scoreSystem) : tileSpawnSystem(tileSpawnSystem), scoreSystem(scoreSystem) {}

void BallMovementSystem::Reset(ZigZagContext& context)
{
    spaceWasPressed = false;
    fellOff = false;
}

void BallMovementSystem::Init(ZigZagContext& context)
{
    Reset(context);
    spaceWasPressed = context.engine->GetInputSystem()->IsKeyDown(Key::Space);

    auto& world = context.engine->GetWorld();
    auto* renderEngine = context.engine->GetRenderEngine();

    unsigned int indexCount = 0;
    const auto vao = MeshFactory::CreateSphere(renderEngine, indexCount, 12, 18, context.ballRadius);

    context.ballEntity = world.CreateEntity();

    auto& transform = world.AddComponent<TransformComponent>(context.ballEntity);
    transform.position = {0.0f, context.platformTopY + context.ballRadius, 0.0f};

    auto& mesh = world.AddComponent<MeshComponent>(context.ballEntity);
    mesh.vao = vao;
    mesh.indexCount = indexCount;

    auto& material = world.AddComponent<MaterialComponent>(context.ballEntity);
    material.diffuseColor = {0.65f, 0.65f, 0.68f, 1.0f};

    auto& shaderComp = world.AddComponent<ShaderComponent>(context.ballEntity);
    shaderComp.shader = context.litShader;
    shaderComp.shaderType = ShaderRenderType::Lit;

    auto& ball = world.AddComponent<ZigZagBallComponent>(context.ballEntity);
    ball.direction = context.forwardDir;

    context.allEntities.push_back(context.ballEntity);
}

void BallMovementSystem::SyncInputEdge(ZigZagContext& context)
{
    spaceWasPressed = context.engine->GetInputSystem()->IsKeyDown(Key::Space);
}

void BallMovementSystem::Update(ZigZagContext& context, float deltaTime)
{
    if (context.ballEntity == 0)
        return;

    const bool spacePressed = context.engine->GetInputSystem()->IsKeyDown(Key::Space);
    const bool spacePressedThisFrame = spacePressed && !spaceWasPressed;
    spaceWasPressed = spacePressed;

    auto& world = context.engine->GetWorld();
    auto& ball = world.GetComponent<ZigZagBallComponent>(context.ballEntity);
    auto& transform = world.GetComponent<TransformComponent>(context.ballEntity);

    if (spacePressedThisFrame && ball.alive)
    {
        const bool goingRight = std::abs(ball.direction.x) > 0.5f;
        ball.direction = goingRight ? context.forwardDir : context.rightDir;
    }

    if (!ball.alive)
    {
        ball.fallSpeed += 12.0f * deltaTime;
        transform.position.y -= ball.fallSpeed * deltaTime;

        if (transform.position.y < context.platformTopY - 6.0f)
            fellOff = true;

        return;
    }

    ball.speed = (std::min)(ball.speed + context.speedGrowthRate * deltaTime, context.maxBallSpeed);

    const float travelDistance = ball.speed * deltaTime;
    transform.position += ball.direction * travelDistance;
    scoreSystem.AddDistanceScore(context, travelDistance);

    size_t tileIndex = SIZE_MAX;
    if (!tileSpawnSystem.IsOverFootprint(context, transform.position, &tileIndex))
        ball.alive = false;
    else if (tileIndex != SIZE_MAX)
        tileSpawnSystem.MaintainPath(context, tileIndex);
}
