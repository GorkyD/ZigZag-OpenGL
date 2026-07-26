#include "GameState/PreStartState.h"

#include <cmath>
#include <memory>

#include "Context/ZigZagContext.h"
#include "Engine/Engine.h"
#include "GameState/GameStateMachine.h"
#include "GameState/PlayingState.h"
#include "Input/InputSystem.h"
#include "Services/HudService.h"
#include "Systems/CameraFollowSystem.h"
#include "Systems/ShadowSystem.h"

PreStartState::PreStartState(GameStateMachine& stateMachine, HudService& hudService, BallMovementSystem& ballMovementSystem, TileSpawnSystem& tileSpawnSystem, CrystalSpawnSystem& crystalSpawnSystem, ParticleBurstSystem& particleBurstSystem,
                             ShadowSystem& shadowSystem, CameraFollowSystem& cameraFollowSystem, ScoreSystem& scoreSystem)
    : stateMachine(stateMachine), hudService(hudService), ballMovementSystem(ballMovementSystem), tileSpawnSystem(tileSpawnSystem), crystalSpawnSystem(crystalSpawnSystem), particleBurstSystem(particleBurstSystem),
      shadowSystem(shadowSystem), cameraFollowSystem(cameraFollowSystem), scoreSystem(scoreSystem)
{
}

void PreStartState::OnEnter(ZigZagContext& context)
{
    hudService.ShowStartHint(context);
    bounceTime = 0.0f;
    spaceWasPressed = false;
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

    if (spacePressedThisFrame)
    {
        hudService.ShowPlayingHint(context);
        stateMachine.ChangeState(context, std::make_unique<PlayingState>(stateMachine, ballMovementSystem, tileSpawnSystem, crystalSpawnSystem, particleBurstSystem, shadowSystem, cameraFollowSystem, scoreSystem, hudService));
    }
}
