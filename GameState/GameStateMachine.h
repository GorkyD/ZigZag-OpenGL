#pragma once

#include "GameState/IGameState.h"
#include <memory>

struct ZigZagContext;

class GameStateMachine
{
public:
    void ChangeState(ZigZagContext& context, std::unique_ptr<IGameState> newState);
    void Update(ZigZagContext& context, float deltaTime);

private:
    std::unique_ptr<IGameState> currentState;
};
