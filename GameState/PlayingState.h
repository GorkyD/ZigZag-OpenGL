#pragma once

#include "GameState/IGameState.h"

class GameStateMachine;
class BallMovementSystem;
class TileSpawnSystem;
class CrystalSpawnSystem;
class ParticleBurstSystem;
class CameraFollowSystem;
class ScoreSystem;
class HudService;
class AutoMoveSystem;

class PlayingState : public IGameState
{
public:
    PlayingState(GameStateMachine& stateMachine, BallMovementSystem& ballMovementSystem, TileSpawnSystem& tileSpawnSystem, CrystalSpawnSystem& crystalSpawnSystem, ParticleBurstSystem& particleBurstSystem,
                 CameraFollowSystem& cameraFollowSystem, ScoreSystem& scoreSystem, HudService& hudService, AutoMoveSystem& autoMoveSystem);

    void OnEnter(ZigZagContext& context) override;
    void OnUpdate(ZigZagContext& context, float deltaTime) override;

private:
    GameStateMachine& stateMachine;
    BallMovementSystem& ballMovementSystem;
    TileSpawnSystem& tileSpawnSystem;
    CrystalSpawnSystem& crystalSpawnSystem;
    ParticleBurstSystem& particleBurstSystem;
    CameraFollowSystem& cameraFollowSystem;
    ScoreSystem& scoreSystem;
    HudService& hudService;
    AutoMoveSystem& autoMoveSystem;
};
