#include "GameState/PlayingState.h"

#include <memory>

#include "GameState/GameOverState.h"
#include "GameState/GameStateMachine.h"
#include "Services/HudService.h"
#include "Systems/BallMovementSystem.h"
#include "Systems/CameraFollowSystem.h"
#include "Systems/CrystalSpawnSystem.h"
#include "Systems/ParticleBurstSystem.h"
#include "Systems/ScoreSystem.h"
#include "Systems/ShadowSystem.h"
#include "Systems/TileSpawnSystem.h"

PlayingState::PlayingState(GameStateMachine& stateMachine, BallMovementSystem& ballMovementSystem, TileSpawnSystem& tileSpawnSystem, CrystalSpawnSystem& crystalSpawnSystem, ParticleBurstSystem& particleBurstSystem,
                           ShadowSystem& shadowSystem, CameraFollowSystem& cameraFollowSystem, ScoreSystem& scoreSystem, HudService& hudService)
    : stateMachine(stateMachine), ballMovementSystem(ballMovementSystem), tileSpawnSystem(tileSpawnSystem), crystalSpawnSystem(crystalSpawnSystem), particleBurstSystem(particleBurstSystem), shadowSystem(shadowSystem),
      cameraFollowSystem(cameraFollowSystem), scoreSystem(scoreSystem), hudService(hudService)
{
}

void PlayingState::OnEnter(ZigZagContext& context)
{
    ballMovementSystem.SyncInputEdge(context);
}

void PlayingState::OnUpdate(ZigZagContext& context, float deltaTime)
{
    ballMovementSystem.Update(context, deltaTime);
    tileSpawnSystem.Update(context, deltaTime);
    crystalSpawnSystem.Update(context, deltaTime);
    particleBurstSystem.Update(context, deltaTime);
    shadowSystem.Update(context, deltaTime);
    cameraFollowSystem.Update(context, deltaTime);
    scoreSystem.Update(context, deltaTime);

    if (ballMovementSystem.HasFallenOff())
        stateMachine.ChangeState(context, std::make_unique<GameOverState>(scoreSystem, hudService));
}
