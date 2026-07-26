#include "Systems/AutoMoveSystem.h"

#include <cmath>

#include "Components/ZigZagBallComponent.h"
#include "Context/ZigZagContext.h"
#include "Ecs/Components/TransformComponent.h"
#include "Ecs/Core/EcsWorld.h"
#include "Engine/Engine.h"
#include "Input/InputSystem.h"
#include "Systems/TileSpawnSystem.h"

AutoMoveSystem::AutoMoveSystem(TileSpawnSystem& tileSpawnSystem) : tileSpawnSystem(tileSpawnSystem) {}

void AutoMoveSystem::Reset(ZigZagContext& context)
{
    enabled = false;
    rWasPressed = false;
}

void AutoMoveSystem::Init(ZigZagContext& context)
{
    Reset(context);
    rWasPressed = context.engine->GetInputSystem()->IsKeyDown(Key::R);
}

void AutoMoveSystem::Enable(ZigZagContext& context)
{
    enabled = true;
    rWasPressed = context.engine->GetInputSystem()->IsKeyDown(Key::R);
}

void AutoMoveSystem::Update(ZigZagContext& context, float deltaTime)
{
    if (context.ballEntity == 0)
        return;

    auto* input = context.engine->GetInputSystem();
    const bool rPressed = input->IsKeyDown(Key::R);
    if (rPressed && !rWasPressed)
        enabled = !enabled;
    rWasPressed = rPressed;

    if (!enabled)
        return;

    auto& world = context.engine->GetWorld();
    auto& ball = world.GetComponent<ZigZagBallComponent>(context.ballEntity);
    if (!ball.alive)
        return;

    auto& transform = world.GetComponent<TransformComponent>(context.ballEntity);

    constexpr float lookaheadMargin = 1.15f;
    const float lookaheadDistance = ball.speed * deltaTime + lookaheadMargin;
    const Vector3 predictedPosition = transform.position + ball.direction * lookaheadDistance;

    if (!tileSpawnSystem.IsOverFootprint(context, predictedPosition, nullptr))
    {
        const bool goingRight = std::abs(ball.direction.x) > 0.5f;
        ball.direction = goingRight ? context.forwardDir : context.rightDir;
    }
}
