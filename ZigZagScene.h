#pragma once

#include "Context/ZigZagContext.h"
#include "GameState/GameStateMachine.h"
#include "Scene/IScene.h"
#include "Services/EnvironmentService.h"
#include "Services/HudService.h"
#include "Systems/BallMovementSystem.h"
#include "Systems/CameraFollowSystem.h"
#include "Systems/CrystalSpawnSystem.h"
#include "Systems/ParticleBurstSystem.h"
#include "Systems/ScoreSystem.h"
#include "Systems/ShadowSystem.h"
#include "Systems/TileSpawnSystem.h"

class ZigZagScene : public IScene
{
public:
    void OnLoad(Engine& engine) override;
    void OnUpdate(Engine& engine, float deltaTime) override;

    Vector4 GetClearColor() const override
    {
        return {0.65f, 0.85f, 1.0f, 1.0f};
    }

private:
    void SetupGame(Engine& engine);
    void RestartGame(Engine& engine);

    ZigZagContext context;
    GameStateMachine stateMachine;

    EnvironmentService environmentService;
    HudService hudService;
    TileSpawnSystem tileSpawnSystem;
    ParticleBurstSystem particleBurstSystem;
    ScoreSystem scoreSystem{hudService};
    BallMovementSystem ballMovementSystem{tileSpawnSystem, scoreSystem};
    CrystalSpawnSystem crystalSpawnSystem{tileSpawnSystem, particleBurstSystem, scoreSystem};
    CameraFollowSystem cameraFollowSystem;
    ShadowSystem shadowSystem;
};
