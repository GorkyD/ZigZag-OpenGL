#include "ZigZagScene.h"

#include <memory>

#include "Ecs/Core/EcsWorld.h"
#include "Engine/Engine.h"
#include "GameState/PreStartState.h"

void ZigZagScene::OnLoad(Engine& engine)
{
    context.engine = &engine;
    context.litShader = engine.GetShaders().lit;
    context.unlitShader = engine.GetShaders().unlit;

    tileSpawnSystem.SetOnTileCreated([this](uint64_t tileId, const Vector3& center) { crystalSpawnSystem.NotifyTileCreated(context, tileId, center); });
    tileSpawnSystem.SetOnTileRetired([this](uint64_t tileId) { crystalSpawnSystem.NotifyTileRetired(context, tileId); });

    SetupGame(engine);
}

void ZigZagScene::SetupGame(Engine& engine)
{
    environmentService.Init(context);
    ballMovementSystem.Init(context);
    crystalSpawnSystem.Init(context);
    tileSpawnSystem.Init(context);
    particleBurstSystem.Init(context);
    shadowSystem.Init(context);
    cameraFollowSystem.Init(context);
    scoreSystem.Init(context);
    hudService.Init(context);

    stateMachine.ChangeState(context, std::make_unique<PreStartState>(stateMachine, hudService, ballMovementSystem, tileSpawnSystem, crystalSpawnSystem, particleBurstSystem, shadowSystem, cameraFollowSystem, scoreSystem));
}

void ZigZagScene::OnUpdate(Engine& engine, float deltaTime)
{
    stateMachine.Update(context, deltaTime);

    if (context.restartRequested)
    {
        context.restartRequested = false;
        RestartGame(engine);
    }
}

void ZigZagScene::RestartGame(Engine& engine)
{
    auto& world = engine.GetWorld();

    for (Entity entity : context.allEntities)
        world.DestroyEntity(entity);
    world.ProcessDeferred();
    context.allEntities.clear();

    SetupGame(engine);
}
