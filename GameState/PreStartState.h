#pragma once

#include "GameState/IGameState.h"

class GameStateMachine;
class HudService;
class BallMovementSystem;
class TileSpawnSystem;
class CrystalSpawnSystem;
class ParticleBurstSystem;
class ShadowSystem;
class CameraFollowSystem;
class ScoreSystem;
class AutoMoveSystem;

class PreStartState : public IGameState
{
public:
    PreStartState(GameStateMachine& stateMachine, HudService& hudService, BallMovementSystem& ballMovementSystem, TileSpawnSystem& tileSpawnSystem, CrystalSpawnSystem& crystalSpawnSystem, ParticleBurstSystem& particleBurstSystem,
                  ShadowSystem& shadowSystem, CameraFollowSystem& cameraFollowSystem, ScoreSystem& scoreSystem, AutoMoveSystem& autoMoveSystem);

    void OnEnter(ZigZagContext& context) override;
    void OnUpdate(ZigZagContext& context, float deltaTime) override;

private:
    GameStateMachine& stateMachine;
    HudService& hudService;
    BallMovementSystem& ballMovementSystem;
    TileSpawnSystem& tileSpawnSystem;
    CrystalSpawnSystem& crystalSpawnSystem;
    ParticleBurstSystem& particleBurstSystem;
    ShadowSystem& shadowSystem;
    CameraFollowSystem& cameraFollowSystem;
    ScoreSystem& scoreSystem;
    AutoMoveSystem& autoMoveSystem;

    float bounceTime = 0.0f;
    bool spaceWasPressed = false;
    bool rWasPressed = false;
};
