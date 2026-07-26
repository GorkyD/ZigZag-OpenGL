#include "GameState/GameOverState.h"

#include "Context/ZigZagContext.h"
#include "Engine/Engine.h"
#include "Input/InputSystem.h"
#include "Services/HudService.h"
#include "Systems/ScoreSystem.h"

GameOverState::GameOverState(ScoreSystem& scoreSystem, HudService& hudService) : scoreSystem(scoreSystem), hudService(hudService) {}

void GameOverState::OnEnter(ZigZagContext& context)
{
    scoreSystem.CommitGameOver(context);
    hudService.ShowGameOver(context);
    spaceWasPressed = false;
}

void GameOverState::OnUpdate(ZigZagContext& context, float deltaTime)
{
    const bool spacePressed = context.engine->GetInputSystem()->IsKeyDown(Key::Space);
    const bool spacePressedThisFrame = spacePressed && !spaceWasPressed;
    spaceWasPressed = spacePressed;

    if (spacePressedThisFrame)
        context.restartRequested = true;
}
