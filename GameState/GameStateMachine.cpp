#include "GameState/GameStateMachine.h"

void GameStateMachine::ChangeState(ZigZagContext& context, std::unique_ptr<IGameState> newState)
{
    currentState = std::move(newState);
    if (currentState)
        currentState->OnEnter(context);
}

void GameStateMachine::Update(ZigZagContext& context, float deltaTime)
{
    if (currentState)
        currentState->OnUpdate(context, deltaTime);
}
