#pragma once

struct ZigZagContext;

class ShadowSystem
{
public:
    void Init(ZigZagContext& context);
    void Update(ZigZagContext& context, float deltaTime);
    void Reset(ZigZagContext& context);
};
