#pragma once

#include "GameState/IGameState.h"

class ScoreSystem;
class HudService;

class GameOverState : public IGameState
{
public:
    GameOverState(ScoreSystem& scoreSystem, HudService& hudService);

    void OnEnter(ZigZagContext& context) override;
    void OnUpdate(ZigZagContext& context, float deltaTime) override;

private:
    ScoreSystem& scoreSystem;
    HudService& hudService;

    bool spaceWasPressed = false;
};
