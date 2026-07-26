#pragma once

struct ZigZagContext;

class IGameState
{
public:
    virtual ~IGameState() = default;

    virtual void OnEnter(ZigZagContext& context) {}
    virtual void OnUpdate(ZigZagContext& context, float deltaTime) {}
};
