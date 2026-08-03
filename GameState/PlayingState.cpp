#include "GameState/PlayingState.h"

#include <memory>

#include "GameState/GameOverState.h"
#include "GameState/GameStateMachine.h"
#include "Services/HudService.h"
#include "Systems/AutoMoveSystem.h"
#include "Systems/BallMovementSystem.h"
#include "Systems/CameraFollowSystem.h"
#include "Systems/CrystalSpawnSystem.h"
#include "Systems/ParticleBurstSystem.h"
#include "Systems/ScoreSystem.h"
#include "Systems/TileSpawnSystem.h"

PlayingState::PlayingState(GameStateMachine& stateMachine, BallMovementSystem& ballMovementSystem, TileSpawnSystem& tileSpawnSystem, CrystalSpawnSystem& crystalSpawnSystem, ParticleBurstSystem& particleBurstSystem,
                           CameraFollowSystem& cameraFollowSystem, ScoreSystem& scoreSystem, HudService& hudService, AutoMoveSystem& autoMoveSystem)
    : stateMachine(stateMachine), ballMovementSystem(ballMovementSystem), tileSpawnSystem(tileSpawnSystem), crystalSpawnSystem(crystalSpawnSystem), particleBurstSystem(particleBurstSystem),
      cameraFollowSystem(cameraFollowSystem), scoreSystem(scoreSystem), hudService(hudService), autoMoveSystem(autoMoveSystem)
{
}

void PlayingState::OnEnter(ZigZagContext& context)
{
    ballMovementSystem.SyncInputEdge(context);
}

void PlayingState::OnUpdate(ZigZagContext& context, float deltaTime)
{
    autoMoveSystem.Update(context, deltaTime);
    ballMovementSystem.Update(context, deltaTime);
    tileSpawnSystem.Update(context, deltaTime);
    crystalSpawnSystem.Update(context, deltaTime);
    particleBurstSystem.Update(context, deltaTime);
    cameraFollowSystem.Update(context, deltaTime);
    scoreSystem.Update(context, deltaTime);

    if (ballMovementSystem.HasFallenOff())
        stateMachine.ChangeState(context, std::make_unique<GameOverState>(scoreSystem, hudService));
}
