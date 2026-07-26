#include "GameState/PreStartState.h"

#include <cmath>
#include <memory>

#include "Context/ZigZagContext.h"
#include "Engine/Engine.h"
#include "GameState/GameStateMachine.h"
#include "GameState/PlayingState.h"
#include "Input/InputSystem.h"
#include "Services/HudService.h"
#include "Systems/AutoMoveSystem.h"
#include "Systems/CameraFollowSystem.h"
#include "Systems/ShadowSystem.h"

PreStartState::PreStartState(GameStateMachine& stateMachine, HudService& hudService, BallMovementSystem& ballMovementSystem, TileSpawnSystem& tileSpawnSystem, CrystalSpawnSystem& crystalSpawnSystem, ParticleBurstSystem& particleBurstSystem,
                             ShadowSystem& shadowSystem, CameraFollowSystem& cameraFollowSystem, ScoreSystem& scoreSystem, AutoMoveSystem& autoMoveSystem)
    : stateMachine(stateMachine), hudService(hudService), ballMovementSystem(ballMovementSystem), tileSpawnSystem(tileSpawnSystem), crystalSpawnSystem(crystalSpawnSystem), particleBurstSystem(particleBurstSystem),
      shadowSystem(shadowSystem), cameraFollowSystem(cameraFollowSystem), scoreSystem(scoreSystem), autoMoveSystem(autoMoveSystem)
{
}

void PreStartState::OnEnter(ZigZagContext& context)
{
    hudService.ShowStartHint(context);
    bounceTime = 0.0f;
    spaceWasPressed = context.engine->GetInputSystem()->IsKeyDown(Key::Space);
    rWasPressed = context.engine->GetInputSystem()->IsKeyDown(Key::R);
}

void PreStartState::OnUpdate(ZigZagContext& context, float deltaTime)
{
    cameraFollowSystem.Update(context, deltaTime);
    shadowSystem.Update(context, deltaTime);

    bounceTime += deltaTime;

    constexpr float bounceSpeed = 6.0f;
    constexpr float bounceAmplitude = 10.0f;
    const float offsetY = std::abs(std::sin(bounceTime * bounceSpeed)) * bounceAmplitude;
    hudService.SetHintBounceOffset(context, offsetY);

    const bool spacePressed = context.engine->GetInputSystem()->IsKeyDown(Key::Space);
    const bool spacePressedThisFrame = spacePressed && !spaceWasPressed;
    spaceWasPressed = spacePressed;

    const bool rPressed = context.engine->GetInputSystem()->IsKeyDown(Key::R);
    const bool rPressedThisFrame = rPressed && !rWasPressed;
    rWasPressed = rPressed;

    if (spacePressedThisFrame || rPressedThisFrame)
    {
        if (rPressedThisFrame)
            autoMoveSystem.Enable(context);

        hudService.ShowPlayingHint(context);
        stateMachine.ChangeState(context, std::make_unique<PlayingState>(stateMachine, ballMovementSystem, tileSpawnSystem, crystalSpawnSystem, particleBurstSystem, shadowSystem, cameraFollowSystem, scoreSystem, hudService, autoMoveSystem));
    }
}
